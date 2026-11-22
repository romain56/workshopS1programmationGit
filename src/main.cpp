#include <sil/sil.hpp>
#include <iostream>
#include "random.hpp"
#include <complex>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/glm.hpp>

//conversions d'espaces de couleurs recupérer sur le site du workshop
glm::vec3 linear_to_srgb(glm::vec3 c) {
    glm::bvec3 cutoff = glm::lessThanEqual(c, glm::vec3(0.0031308f));
    glm::vec3 low = c * 12.92f;
    glm::vec3 high = 1.055f * glm::pow(c, glm::vec3(1.0f / 2.4f)) - 0.055f;
    return glm::mix(high, low, cutoff);
}


glm::vec3 linear_srgb_to_oklab(glm::vec3 c) 
{
    float l = 0.4122214708f * c.r + 0.5363325363f * c.g + 0.0514459929f * c.b;
	float m = 0.2119034982f * c.r + 0.6806995451f * c.g + 0.1073969566f * c.b;
	float s = 0.0883024619f * c.r + 0.2817188376f * c.g + 0.6299787005f * c.b;

    float l_ = cbrtf(l);
    float m_ = cbrtf(m);
    float s_ = cbrtf(s);

    return {
        0.2104542553f*l_ + 0.7936177850f*m_ - 0.0040720468f*s_,
        1.9779984951f*l_ - 2.4285922050f*m_ + 0.4505937099f*s_,
        0.0259040371f*l_ + 0.7827717662f*m_ - 0.8086757660f*s_,
    };
}

glm::vec3 oklab_to_linear_srgb(glm::vec3 c) 
{
    float l_ = c.r + 0.3963377774f * c.g + 0.2158037573f * c.b;
    float m_ = c.r - 0.1055613458f * c.g - 0.0638541728f * c.b;
    float s_ = c.r - 0.0894841775f * c.g - 1.2914855480f * c.b;

    float l = l_*l_*l_;
    float m = m_*m_*m_;
    float s = s_*s_*s_;

    return {
		+4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
		-1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
		-0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s,
    };
}

//ne garde que le canal vert
sil::Image vert(sil::Image image)
{
    
    sil::Image cp {image};
    const int w {image.width()};
    int g;
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < image.height(); y++)
            {
                cp.pixel(x, y).r = 0;
                cp.pixel(x, y).g = image.pixel(x,y).g;
                cp.pixel(x, y).b = 0;
            }
    }
    return cp;
}

//echange les canaux rouge et bleu
sil::Image echanger(sil::Image image)
{
    
    sil::Image cp {image};
    const int w {image.width()};
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < image.height(); y++)
            {
                cp.pixel(x, y).r = image.pixel(x,y).b;
                cp.pixel(x, y).g = image.pixel(x,y).g;
                cp.pixel(x, y).b = image.pixel(x,y).r;
            }
    }
    return cp;
}

//passage en niveaux de gris
sil::Image gris(sil::Image image)
{
    
    sil::Image cp {image};
    const int w {image.width()};
    float g;
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < image.height(); y++)
            {
                //formule du workshop.
                g = 0.299  * cp.pixel(x, y).r + 0.587 * cp.pixel(x, y).g + 0.114 * cp.pixel(x, y).b;
                cp.pixel(x, y).r = g;
                cp.pixel(x, y).g = g;
                cp.pixel(x, y).b = g;
            }
    }
    return cp;
}

//inverse les couleurs
sil::Image neg(sil::Image image)
{
    
    sil::Image cp {image};
    const int w {image.width()};
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < image.height(); y++)
            {
                // les valeurs etant comprise entre 0 et 1
                cp.pixel(x, y).r = 1 - image.pixel( x , y ).r ;
                cp.pixel(x, y).g = 1 - image.pixel( x , y ).g ;
                cp.pixel(x, y).b = 1 - image.pixel( x , y ).b ;
            }
    }
    return cp;
}

//degrade horizontal du noir au blanc
sil::Image degrade()
{
    
    sil::Image image{300, 200};
    const float w {300};
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < image.height(); y++)
            {
                //formule de noir au blanc.
                float v = 1 - ( w - x - 1)/ w;
                image.pixel(x, y).r = v ;
                image.pixel(x, y).g = v;
                image.pixel(x, y).b = v;
            }
    }
    return image;
}

//effet miroir horizontal
sil::Image mirror(sil::Image image)
{
    
    sil::Image cp {image};
    const int w {image.width()};
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < image.height(); y++)
            {
                cp.pixel(w-x-1, y) = image.pixel(x,y);
            }
    }
    return cp;
}

//ajoute du bruit aleatoire sur 30% des pixels
sil::Image alea(sil::Image image)
{
    
    sil::Image cp {image};
    const int w {image.width()};
     for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < image.height(); y++)
            {
                if( random_float(0,1) < 0.3){
                    cp.pixel(x, y).r = random_float(0,1);
                    cp.pixel(x, y).g = random_float(0,1);
                    cp.pixel(x, y).b = random_float(0,1);
                }
                else{
                    cp.pixel( x , y ) = image.pixel( x , y );
                }
                   
            }
    }
    return cp;
}

//rotation de 90 degres
sil::Image rotate90(sil::Image image)
{
    
    sil::Image cp { image.height() , image.width() };
    const int w {image.width()};
    const int h {image.height()};
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
                // h -1 donne la hauteur et -y l'inverse de sa hauteur de base
                cp.pixel( h - 1 - y , x ) = image.pixel( x , y );
            }
    }
    return cp;
}

//decale les canaux rouge et bleu
sil::Image split(sil::Image image)
{
    
    sil::Image cp {image};
    const int w {image.width()};
    const int h {image.height()};
    int seuil {30};
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
                //on ajoute un seuil en fonction de si le pixel est dans l'image
                if( x > w - seuil - 1){
                    cp.pixel( x , y ).b = 0;
                }
                else{
                    cp.pixel(x, y).b = image.pixel(x + seuil, y).b;
                }

                cp.pixel(x, y).g = image.pixel(x, y).g;
                
                if ( x < seuil ){
                    cp.pixel(x, y).r = 0; 
                }
                else{
                    cp.pixel(x, y).r = image.pixel(x - seuil, y).r; 
                }


        }
    }
    return cp;
}

//modifie la luminosite avec une puissance
sil::Image assombrissement(sil::Image image , float seuil )
{
    
    sil::Image cp {image};
    const int w {image.width()};
    const int h {image.height()};
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
            cp.pixel(x, y).r = std :: pow( image.pixel(x, y).r , seuil) ;
            cp.pixel(x, y).g = std :: pow( image.pixel(x, y).g , seuil) ;
            cp.pixel(x, y).b = std :: pow( image.pixel(x, y).b , seuil); 
        }
    }
    return cp;
}

//dessine un disque plein au centre
sil::Image disk( float R)
{
    
    sil::Image d { 500 , 500 };
    for (int x{0}; x < 500; x++)
        {
        for (int y{0}; y < 500; y++)
            {
            //si il est dans le centre
            if ( std :: pow(std :: abs (x  - 250) , 2) + std :: pow( std :: abs(y  -250) , 2 ) <= R*R){
                d.pixel(x, y).r = 255;
                d.pixel(x, y).g = 255;
                d.pixel(x, y).b = 255;
            }
                 
        }
    }
    return d;
}

//dessine un cercle avec une epaisseur
sil::Image circle( float R , float thickness )
{
    
    sil::Image d { 500 , 500 };
    for (int x{0}; x < 500; x++)
        {
        for (int y{0}; y < 500; y++)
            {
            float res = std :: pow(std :: abs (x  - 250) , 2) + std :: pow( std :: abs(y  -250) , 2 );
            //si il est dans le cercle
            if ( res <= R*R  && res >= (R*R - thickness)){
                d.pixel(x, y).r = 255;
                d.pixel(x, y).g = 255;
                d.pixel(x, y).b = 255;
            }
                 
        }
    }
    return d;
}

//disque decale en x pour l'animation
sil::Image anim( float R , float centerX)
{
    const float posX =  centerX - 250;

    sil::Image d { 500 , 500 };
    for (int x{0}; x < 500; x++)
        {
        for (int y{0}; y < 500; y++)
            {
            //si il est dans le centre et avance le centre
            if ( std :: pow(std :: abs (x  - 250) , 2) + std :: pow( std :: abs(y  -250) , 2 ) <= R*R){
                float new_x = x + posX;
                if( new_x >= 0 && new_x < 500){
                    d.pixel(new_x, y).r = 255;
                    d.pixel(new_x, y).g = 255;
                    d.pixel(new_x, y).b = 255;
                }
            }
                 
        }
    }
    return d;
}

//dessine une rosace de 6 cercles
sil::Image rosas( float R , float thickness )
{
    float const pi = std::acos(-1.0);
    float const angle = pi /3.f;
    sil::Image d { 500 , 500 };
    for (int x{0}; x < 500; x++)
        {
        for (int y{0}; y < 500; y++)
            {
            float res = std :: pow(std :: abs (x  - 250) , 2) + std :: pow( std :: abs(y  -250) , 2 );
            //si il est dans le centre qui sert de base
            if ( res <= R*R  && res >= (R*R - thickness)){
                //dessine chaque cercle 1 par 1
                for ( int i{0}; i<6; i++){
                    float coodX = std :: cos( angle * i ) * R;
                    float coodY = std :: sin( angle * i ) * R;
                    if (y + coodY >= 0 && y + coodY < 500 && x + coodX >= 0 && x + coodX < 500){
                        d.pixel(x + coodX, y + coodY).r = 255;
                        d.pixel(x + coodX, y + coodY).g = 255;
                        d.pixel(x + coodX, y + coodY).b = 255;
                    }
                        
                }
            }
                 
        }
    }
    return d;
}

//repete l'image en mosaique simple
sil::Image modulo(sil::Image image , int seuil )
{
    
    sil::Image cp {image};
    const int w {image.width()};
    const int h {image.height()};

    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
            //force a etre un pixel de l'image
            float y_modulo = (seuil * y )% h;  
            float x_modulo = (seuil * x ) % w;
            cp.pixel(x, y).r = image.pixel( x_modulo ,y_modulo).r;
            cp.pixel(x, y).g = image.pixel( x_modulo ,y_modulo).b;
            cp.pixel(x, y).b = image.pixel( x_modulo ,y_modulo).g; 
        }
    }
    return cp;
}

//mosaique avec effet miroir sur les bords
sil::Image mosaique(sil::Image image , int seuil )
{
    
    sil::Image cp {image};
    const int w {image.width()};
    const int h {image.height()};

    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
            
            //force a etre un pixel de l'image
            float y_modulo = (seuil * y ) % (2 * h);  
            float x_modulo = (seuil * x ) % (2 * w);

            //inversion une fois sur deux
            if ( y_modulo >= h){
                y_modulo = 2*h - y_modulo - 1;
            }
            if ( x_modulo >= w){
                x_modulo = 2*w - x_modulo - 1;
            }
            cp.pixel(x, y).r = image.pixel( x_modulo ,y_modulo).r;
            cp.pixel(x, y).g = image.pixel( x_modulo ,y_modulo).b;
            cp.pixel(x, y).b = image.pixel( x_modulo ,y_modulo).g; 
        }
    }
    return cp;
}

//echange des rectangles aleatoires pour l'effet glitch
sil::Image glitch(sil::Image image , int nbRect )
{
    
    sil::Image cp {image};
    const int w {image.width()};
    const int h {image.height()};
    
    for (int i {0}; i < nbRect ; i++){
        int l_x {random_int(5, 30)};
        int l_y {random_int(1, 5)};
        int randomX {random_int(0, w - l_x )};
        int randomY {random_int(0, h - l_y )};

        int debX {random_int(0, w - l_x)}; 
        int debY {random_int(0, h - l_y)}; 
        //echange le rectangle avec le rectangle aleatoire
        for (int x{debX}; x < debX + l_x ; x++)
        {
        for (int y{debY}; y < debY + l_y ; y++)
            {
                int currentX {randomX + (x-debX)};
                int currentY {randomY + (y-debY)};
                cp.pixel(x, y).r = image.pixel( currentX , currentY ).r;
                cp.pixel(x, y).g = image.pixel( currentX , currentY ).b;
                cp.pixel(x, y).b = image.pixel( currentX , currentY ).g;
                cp.pixel( currentX , currentY ).r = image.pixel( x , y ).r;
                cp.pixel( currentX , currentY ).g = image.pixel(  x , y ).b;
                cp.pixel( currentX , currentY ).b = image.pixel(  x , y ).g;  
            }
        }


    }
    return cp;
}

//calcul de la luminosite d'un pixel
float brightness ( glm :: vec3 c){
    return 0.212*c.r +0.7152*c.g + 0.0722*c.b ;
}

//trie les pixels par luminosite
sil::Image trie(sil::Image image)
{

    std :: vector<glm :: vec3> v { image.pixels() };
    std::sort(v.begin(), v.end(), [](glm::vec3 const& color1, glm::vec3 const& color2)
    {
        return brightness(color1) < brightness(color2); 
    });
    image.pixels() = v;
    
    return image;
}

//fractale de mandelbrot
sil::Image mandelbroot( )
{

    std::complex<float> z{3.f, 2.f};
    sil::Image cp {500,500};
    const float w {500};
    const float h {500};
    for (float x{0}; x < w; x++)
        {
        for (float y{0}; y < h; y++)
            {
                std::complex<float> z{ 0, 0 };
                std::complex<float> c{ (x-250)/125.f, (y-250)/125.f };
                int i{0};
                
                //verifie si z diverge
                while (std::abs(z) < 2 && i < 50){
                    z = z*z + c ;
                    i++;
                }
                
                //si on est sorti de la boucle car z diverge
                if( i < 50){
                    cp.pixel(x,y).r = static_cast<float>(i)/50.f;
                    cp.pixel(x,y).b = static_cast<float>(i)/50.f;
                    cp.pixel(x,y).g = static_cast<float>(i)/50.f;
                }
                else {
                    cp.pixel(x,y).r = 1;
                    cp.pixel(x,y).b = 1;
                    cp.pixel(x,y).g = 1;
                }
        }
    }
    return cp;
}

//tramage avec matrice de bayer
sil::Image tramage(sil::Image image )
{
    
    sil::Image cp {image};
    const int w {image.width()};
    const int h {image.height()};

    //matrice trouver sur le site du workshop
    const int bayer_n = 4;
    float bayer_matrix_4x4[bayer_n][bayer_n] = {
        {    -0.5,       0,  -0.375,   0.125 },
        {    0.25,   -0.25,   0.375, - 0.125 },
        { -0.3125,  0.1875, -0.4375,  0.0625 },
        {  0.4375, -0.0625,  0.3125, -0.1875 },
    };

    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
            
            float g = 0.299  * cp.pixel(x, y).r + 0.587 * cp.pixel(x, y).g + 0.114 * cp.pixel(x, y).b ;
            float coeff = bayer_matrix_4x4[ x%bayer_n ][ y%bayer_n ];
            if (g < coeff){
                cp.pixel(x, y).r = 0;
                cp.pixel(x, y).g = 0;
                cp.pixel(x, y).b = 0;
            }
            else{
                cp.pixel(x, y).r = 1;
                cp.pixel(x, y).g = 1;
                cp.pixel(x, y).b = 1;
            }
                 
        }
    }
    return cp;
}

//normalisation de l'histogramme
sil::Image normalisation( sil :: Image image)
{
    
    sil::Image cp {image};
    const int w {image.width()};
    const int h {image.height()};
    float brightnessMax {0};
    float brightnessMin {1};

    //le calcul du min et max
    for (float x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
            float brightnessCurrent = brightness(cp.pixel(x,y));
            if ( brightnessCurrent >= brightnessMax){
                brightnessMax = brightnessCurrent;
            }
            if ( brightnessCurrent <= brightnessMin){
                brightnessMin = brightnessCurrent;
            }
           
        }
    }
    
    //puis on normalise
    for (float x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
            
            cp.pixel( x, y ).r = (cp.pixel( x, y ).r - brightnessMin) / (brightnessMax - brightnessMin);
            cp.pixel( x, y ).g = (cp.pixel( x, y ).g - brightnessMin) / (brightnessMax - brightnessMin);;
            cp.pixel( x, y ).b = (cp.pixel( x, y ).b - brightnessMin) / (brightnessMax - brightnessMin);;
            
        }
    }
    
    return cp;
}

//degrade du rouge au vert en passant par oklab
sil::Image degradeRGB( )
{
    
    sil::Image cp {500,500};
    const float w {500};
    const float h {500};
    glm::vec3 vertenoklab = linear_srgb_to_oklab( glm :: vec3 {0, 1,  0});
    glm::vec3 rougeenoklab = linear_srgb_to_oklab( glm :: vec3 {1, 0,  0});
    for (int y{0}; y < h; y++)
        {
        cp.pixel(0, y).r = 1 ;
        cp.pixel(0, y).g =  0;
    }
    for (float x{1}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
            //mix en oklab
            glm::vec3 mix_oklab = glm :: mix(rougeenoklab, vertenoklab, x/w ) ;
            
            //transvation d'espace
            glm :: vec3 mix_lrgb = oklab_to_linear_srgb(mix_oklab);
            glm :: vec3 mix_srgb = linear_to_srgb(mix_lrgb);
            
            cp.pixel(x, y).r = mix_srgb.r ;
            cp.pixel(x, y).g =  mix_srgb.g ;
           
        }
    }
    return cp;
}

//rotation d'un point autour d'un centre
glm::vec2 rotated(glm::vec2 point, glm::vec2 center_of_rotation, float angle)
{
    return glm::vec2{glm::rotate(glm::mat3{1.f}, angle) * glm::vec3{point - center_of_rotation, 0.f}} + center_of_rotation;
}

//effet vortex en fonction de la distance au centre
sil::Image vortex(sil::Image image )
{
    
    sil::Image cp {image};
    const int w {image.width()};
    const int h {image.height()};
    const glm::vec2 center{w/2.f , h/2.f};
    for (float x{0}; x < w; x++)
        {
        for (float y{0}; y < h; y++)
            {
            
            // tourne l'image en fonction du centre
            const glm::vec2 p = rotated( {x,y} , center, glm::distance(center , {x,y} )/16.f);
            if (p.x >= 0 && p.y >=0 && p.y < h && p.x < w){
                cp.pixel(x, y) = image.pixel(p.x,p.y);
            }
            else{
                 cp.pixel(x, y) = {0,0,0};
            }
                

        }
    }
    return cp;
}

//flou simple par moyenne des voisins sans matrice
sil::Image convolution( sil::Image image )
{
    
    const int w {image.width()};
    const int h {image.height()};
    sil::Image cp { w, h};
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
            for (int i{-1}; i <= 1; i++)
                {
                for (int j{-1}; j <= 1; j++){
                    int new_x = i + x;
                    int new_y = j + y; 
                    if( new_x  >= 0 && new_x < w && new_y >= 0 && new_y < h){
                        cp.pixel(x, y).r += image.pixel(new_x  , new_y).r/9.f;
                        cp.pixel(x, y).g += image.pixel(new_x , new_y).g/9.f;
                        cp.pixel(x, y).b += image.pixel(new_x , new_y).b/9.f;
                    }

                }
            }
            
        }
    }
    return cp;
}

//applique un noyau de convolution donnée
sil::Image convolutionKernel( sil::Image image , std :: vector<std :: vector<float>> kernel)
{
    int n = kernel.size();
    int mid = n/2; 
    const int w {image.width()};
    const int h {image.height()};
    sil::Image cp { w, h};
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {
            // applique le noyau a chaque pixel
            for (int i{0}; i < n; i++)
                {
                for (int j{0}; j < n; j++){
                    //determine la position du pixel auquel on applique la transformation i j du noyau
                    int new_x = i + x - mid;
                    int new_y = j + y - mid; 
                    float currentKernel = kernel[ i ][ j];
                    if( new_x  >= 0 && new_x < w && new_y >= 0 && new_y < h){
                        cp.pixel(x, y).r += (image.pixel(new_x  , new_y).r * currentKernel );
                        cp.pixel(x, y).g += (image.pixel(new_x , new_y).g * currentKernel );
                        cp.pixel(x, y).b += (image.pixel(new_x , new_y).b * currentKernel );
                    }

                }
            }
            
        }
    }
    return cp;
}

//filtre kuwahara effet peinture
sil::Image kuwahara( sil::Image image )
{
    glm :: vec3 currentSum, average, pixelMin;
    float min, averageDist;
    int n = 5;
    int mid = n/2; 
    const int w {image.width()};
    const int h {image.height()};
    sil::Image cp { w, h};
    
    for (int x{0}; x < w; x++)
        {
        for (int y{0}; y < h; y++)
            {

            
            averageDist = 0.f;
            currentSum = { 0, 0, 0 };
            //1er carré
            //calcul la couleur moyenne 
            for (int i{0}; i < mid; i++)
                {
                for (int j{0}; j < mid; j++){
                    int new_x = ( i - mid ) + x ;
                    int new_y = ( j - mid ) + y ; 
                   
                    if( new_x  >= 0 && new_x < w && new_y >= 0 && new_y < h){
                        currentSum+= image.pixel( new_x , new_y );
                    }
                }
            }
            average = currentSum * 0.25f;
            
            //calcul la distance moyenne entre chaque pixel
            for (int i{0}; i < mid; i++)
                {
                for (int j{0}; j < mid; j++){
                    int new_x = ( i - mid ) + x ;
                    int new_y = ( j - mid ) + y ; 
                   
                    if( new_x  >= 0 && new_x < w && new_y >= 0 && new_y < h){
                        averageDist += glm :: distance ( image.pixel( new_x, new_y), average);
                    }
                }
            }
            averageDist = averageDist * 0.25f;
            min = averageDist;
            pixelMin = average;

            averageDist = 0.f;
            currentSum = { 0, 0, 0 };
            //2eme carré
            for (int i{0}; i < mid; i++)
                {
                for (int j{mid+1}; j < n; j++){
                    int new_x = ( i - mid ) + x ;
                    int new_y = ( j - mid ) + y ; 
                   
                    if( new_x  >= 0 && new_x < w && new_y >= 0 && new_y < h){
                        currentSum+= image.pixel( new_x , new_y );
                    }
                }
            }
           average = currentSum * 0.25f;
           for (int i{0}; i < mid; i++)
                {
                for (int j{mid+1}; j < n; j++){
                    int new_x = ( i - mid ) + x ;
                    int new_y = ( j - mid ) + y ; 
                   
                    if( new_x  >= 0 && new_x < w && new_y >= 0 && new_y < h){
                        averageDist += glm :: distance ( image.pixel( new_x, new_y), average);
                    }
                }
            }
            averageDist = averageDist * 0.25f;
            if (averageDist < min) {
                min = averageDist;
                pixelMin = average;
            }

            averageDist = 0.f;
            currentSum = { 0, 0, 0 };
            //3eme carré
            for (int i{mid+1}; i < n; i++)
                {
                for (int j{0}; j < mid; j++){
                    int new_x = ( i - mid ) + x ;
                    int new_y = ( j - mid ) + y ; 
                   
                    if( new_x  >= 0 && new_x < w && new_y >= 0 && new_y < h){
                        currentSum+= image.pixel( new_x , new_y );
                    }
                }
            }

            average = currentSum * 0.25f;
            for (int i{mid+1}; i < n; i++)
                {
                for (int j{0}; j < mid; j++){
                    int new_x = ( i - mid ) + x ;
                    int new_y = ( j - mid ) + y ; 
                   
                    if( new_x  >= 0 && new_x < w && new_y >= 0 && new_y < h){
                        averageDist += glm :: distance ( image.pixel( new_x, new_y), average);
                    }
                }
            }
            averageDist = averageDist * 0.25f;
            if (averageDist < min) {
                min = averageDist;
                pixelMin = average;
            }
            
            averageDist = 0.f;
            currentSum = { 0, 0, 0 };
            //4eme carré
            for (int i{mid+1}; i < n; i++)
                {
                for (int j{mid+1}; j < n; j++){
                    int new_x = ( i - mid ) + x ;
                    int new_y = ( j - mid ) + y ; 
                   
                    if( new_x  >= 0 && new_x < w && new_y >= 0 && new_y < h){
                        currentSum+= image.pixel( new_x , new_y );
                    }
                }
            }
            average = currentSum * 0.25f;
            for (int i{mid+1}; i < n; i++)
                {
                for (int j{mid+1}; j < n; j++){
                    int new_x = ( i - mid ) + x ;
                    int new_y = ( j - mid ) + y ; 
                   
                    if( new_x  >= 0 && new_x < w && new_y >= 0 && new_y < h){
                        averageDist += glm :: distance ( image.pixel( new_x, new_y), average);
                    }
                }
            }
            averageDist = averageDist * 0.25f;
            if (averageDist < min) {
                min = averageDist;
                pixelMin = average;
            }
            cp.pixel( x, y) = pixelMin;
        }
    }
    return cp;
}


//etape recursive de l'algo diamond square
sil::Image diamondSquareRec( float step, sil::Image image, float ecart)
{ 
    float centerX, centerY, r, colorCenter, color1, color2, color3, color4, colorMeans;
    
    if(step<2)
        return image;

    float mid {step/2.f};
    //etape carre
    for(int i{0}; i < image.width() - step; i = i + step){
        for(int j{0}; j < image.height() - step ; j= j + step){
            centerX = i + mid;
            centerY = j + mid;
            float startW { static_cast<float>(i)};
            float startH { static_cast<float>(j)};
            float endW {i+step};
            float endH {j+step};
            color1 = image.pixel( startW,  startH ).r;
            color2 = image.pixel( endW,  startH ).r;
            color3 = image.pixel( startW,  endH ).r;
            color4 = image.pixel( endW,  endH ).r;

            //generation de l'aleatoire dans les couleurs
            float r{random_float(-ecart, ecart)};
            colorCenter = (color4 + color3 + color2 + color1 )/4.f + r ;

            image.pixel( centerX,  centerY ) = {colorCenter, colorCenter, colorCenter};

        }
    }

    //etape losange
    for(float i{0}; i < image.width() -step ; i = i + step){
        for(float j{0}; j < image.height() -step ; j = j + step){
            
            centerX = i + mid;
            centerY = j + mid;
            float count{ 3.f };
            float startW { static_cast<float>(i)};
            float startH { static_cast<float>(j)};
            float endW {i+step};
            float endH {j+step};
            colorCenter = image.pixel( centerX,  centerY ).r;
            color1 = image.pixel( startW,  startH ).r;
            color2 = image.pixel( endW,  startH ).r;
            color3 = image.pixel( startW,  endH ).r;
            color4 = image.pixel( endW,  endH ).r;

            //losange milieu gauche
            colorMeans = 0.f;
            r = random_float(-ecart, ecart);
            //si le voisin est bien dans l'espace visible
            if (startW -  mid > 0){
                count++;
                colorMeans += image.pixel(startW -  mid , centerY).r;
            }
            colorMeans = (colorMeans + color1 + color3 + colorCenter)/count + r ;
            image.pixel(startW, centerY) = {colorMeans, colorMeans, colorMeans};

            //losange milieu haut
            count = 3.f ;
            colorMeans = 0.f;
            r = random_float(-ecart, ecart);
            if (startH -  mid > 0){
                count++;
                colorMeans += image.pixel( centerX, startH -  mid).r;
            }
            colorMeans = (colorMeans + color1 + color2 + colorCenter)/count + r;
            image.pixel(centerX, startH) = {colorMeans, colorMeans, colorMeans};

            //losange milieu droit
            count = 3.f ;
            colorMeans = 0.f;
            r = random_float(-ecart, ecart);
            if(endW + mid < image.width()){
                count++;
                colorMeans += image.pixel( endW + mid, centerY).r;
            }
            colorMeans = (colorMeans + color4 + color2 + colorCenter )/count + r;
            image.pixel(endW, centerY) = {colorMeans, colorMeans, colorMeans} ;

            //losange milieu bas
            count = 3.f ;
            colorMeans = 0.f;
            r = random_float(-ecart, ecart);
            if(endH + mid < image.height()){
                count++;
                colorMeans += image.pixel(centerX, endH + mid).r;
            }
            colorMeans = (colorMeans + color4 + color3 + colorCenter)/count + r;
            image.pixel(centerX, endH) = { colorMeans, colorMeans, colorMeans};
        }
    }
    ecart *= 0.5f;
    return diamondSquareRec( mid, image, ecart );    
}

//initialisation des 4 coins pour diamond square
sil::Image diamondSquare( )
{  
    
    float color1, color2, color3, color4;
    float n {513};
    sil::Image image { 513 , 513 };
    const float w { n };
    color1 = random_float(0,1);
    image.pixel( 0,  0 ) = { color1, color1, color1 };
    color2 = random_float(0,1);
    image.pixel( 0,  w-1 ) = { color2, color2, color2 };
    color3 = random_float(0,1);
    image.pixel( w-1,  0 ) = { color3, color3, color3 };
    color4 = random_float(0,1);
    image.pixel( w-1,  w-1 ) = { color4, color4, color4 };

    //ensuite on lance la recurrence
    return diamondSquareRec( n-1, image, 1.f);
}



//etape recursive de l'algo diamond square
sil::Image diamondSquareColorRec( float step, sil::Image image, float ecart)
{ 
    float centerX, centerY, r, colorCenter, color1, color2, color3, color4, colorMeans;
    
    if(step<2){
        for(float i{0}; i < image.width() ; i = i + 1){
            for(float j{0}; j < image.height() ; j = j + 1){
                float color {image.pixel( i, j ).r};
                if(color > 0.5f){
                    image.pixel( i, j ) = glm::mix(glm :: vec3 {0.f,1.f,0.f}, glm :: vec3 {0.f,0.4f,0.f}, color);
                }
                else{
                    image.pixel( i, j ) = glm :: mix(glm :: vec3 {0.f,0.f,0.1f}, glm :: vec3 {0.4f,0.6f,1.f}, color);
                }
            }
        }
        return image;
    }
        
    float mid {step/2.f};
    //etape carre
    for(int i{0}; i < image.width() - step; i = i + step){
        for(int j{0}; j < image.height() - step ; j= j + step){
            centerX = i + mid;
            centerY = j + mid;
            float startW { static_cast<float>(i)};
            float startH { static_cast<float>(j)};
            float endW {i+step};
            float endH {j+step};
            color1 = image.pixel( startW,  startH ).r;
            color2 = image.pixel( endW,  startH ).r;
            color3 = image.pixel( startW,  endH ).r;
            color4 = image.pixel( endW,  endH ).r;

            //generation de l'aleatoire dans les couleurs
            float r{random_float(-ecart, ecart)};
            colorCenter = (color4 + color3 + color2 + color1 )/4.f + r ;

            image.pixel( centerX,  centerY ) = {colorCenter, colorCenter, colorCenter};

        }
    }

    //etape losange
    for(float i{0}; i < image.width() -step ; i = i + step){
        for(float j{0}; j < image.height() -step ; j = j + step){
            
            centerX = i + mid;
            centerY = j + mid;
            float count{ 3.f };
            float startW { static_cast<float>(i)};
            float startH { static_cast<float>(j)};
            float endW {i+step};
            float endH {j+step};
            colorCenter = image.pixel( centerX,  centerY ).r;
            color1 = image.pixel( startW,  startH ).r;
            color2 = image.pixel( endW,  startH ).r;
            color3 = image.pixel( startW,  endH ).r;
            color4 = image.pixel( endW,  endH ).r;

            //losange milieu gauche
            colorMeans = 0.f;
            r = random_float(-ecart, ecart);
            //si le voisin est bien dans l'espace visible
            if (startW -  mid > 0){
                count++;
                colorMeans += image.pixel(startW -  mid , centerY).r;
            }
            colorMeans = (colorMeans + color1 + color3 + colorCenter)/count + r ;
            image.pixel(startW, centerY) = {colorMeans, colorMeans, colorMeans};

            //losange milieu haut
            count = 3.f ;
            colorMeans = 0.f;
            r = random_float(-ecart, ecart);
            if (startH -  mid > 0){
                count++;
                colorMeans += image.pixel( centerX, startH -  mid).r;
            }
            colorMeans = (colorMeans + color1 + color2 + colorCenter)/count + r;
            image.pixel(centerX, startH) = {colorMeans, colorMeans, colorMeans};

            //losange milieu droit
            count = 3.f ;
            colorMeans = 0.f;
            r = random_float(-ecart, ecart);
            if(endW + mid < image.width()){
                count++;
                colorMeans += image.pixel( endW + mid, centerY).r;
            }
            colorMeans = (colorMeans + color4 + color2 + colorCenter )/count + r;
            image.pixel(endW, centerY) = {colorMeans, colorMeans, colorMeans} ;

            //losange milieu bas
            count = 3.f ;
            colorMeans = 0.f;
            r = random_float(-ecart, ecart);
            if(endH + mid < image.height()){
                count++;
                colorMeans += image.pixel(centerX, endH + mid).r;
            }
            colorMeans = (colorMeans + color4 + color3 + colorCenter)/count + r;
            image.pixel(centerX, endH) = { colorMeans, colorMeans, colorMeans};
        }
    }
    ecart *= 0.5f;
    return diamondSquareColorRec( mid, image, ecart );    
}

//initialisation des 4 coins pour diamond square
sil::Image diamondSquareColor( )
{  
    
    float color1, color2, color3, color4;
    float n {513};
    sil::Image image { 513 , 513 };
    const float w { n };
    color1 = random_float(0,1);
    image.pixel( 0,  0 ) = { color1, color1, color1 };
    color2 = random_float(0,1);
    image.pixel( 0,  w-1 ) = { color2, color2, color2 };
    color3 = random_float(0,1);
    image.pixel( w-1,  0 ) = { color3, color3, color3 };
    color4 = random_float(0,1);
    image.pixel( w-1,  w-1 ) = { color4, color4, color4 };

    //ensuite on lance la recurrence
    return diamondSquareColorRec( n-1, image, 1.f);
}

int main()
{
    //chargement des images de base
    sil::Image image{"images/logo.png"};
    sil::Image photo{"images/photo.jpg"};
    sil::Image photoFaible{"images/photo_faible_contraste.jpg"};

    image.save("output/pouet.png");
    
    //application de chaque fonction 
    sil::Image imageVert = vert(image);
    imageVert.save("output/vert.png");

    sil::Image imageEchanger = echanger(image);
    imageEchanger.save("output/echanger.png");

    sil::Image imageNeg = neg(image);
    imageNeg.save("output/negatif.png");

    sil::Image degradé = degrade();
    degradé.save("output/degrade.png");

    sil::Image imageMirroir = mirror(image);
    imageMirroir.save("output/mirroir.png");

    sil::Image imageAlea = alea(image);
    imageAlea.save("output/alea.png");

    sil::Image image90 = rotate90(image);
    image90.save("output/90.png");

    sil::Image imageSplit = split(image);
    imageSplit.save("output/split.png");

    sil::Image imageAssombrissement = assombrissement(photo , 5);
    imageAssombrissement.save("output/assombrissement.jpg");

    sil::Image imageEclaircit = assombrissement(photo , 0.2);
    imageEclaircit.save("output/eclaircit.jpg");

    sil::Image disque = disk( 30);
    disque.save("output/disk.png");

    sil::Image cercle = circle( 30 , 60);
    cercle.save("output/circle.png");
    

    //generation de l'animation
    /*
    for(int x{469}; x<500;x++){
        sil::Image animation = anim( 30 , x);
        animation.save("output/anim/" + std :: to_string(x) + "anim.png");
    }
    */

    sil::Image dessinRosas = rosas( 60 , 200);
    dessinRosas.save("output/rosas.png");

    sil::Image imageModulo = modulo( image , 5);
    imageModulo.save("output/modulo.png");

    sil::Image mosaiqueMirroir = mosaique( image , 5);
    mosaiqueMirroir.save("output/mosaiqueMirroir.png");

    sil::Image imageGlitch = glitch( image , 200);
    imageGlitch.save("output/glitch.png");
    
    sil::Image imageTrie = trie( image );
    imageTrie.save("output/trie.png");
    
    sil::Image dessinMandelbroot = mandelbroot( );
    dessinMandelbroot.save("output/mandelbroot/mandelbroot.png");

    sil::Image dessinDegradeRGB = degradeRGB();
    dessinDegradeRGB.save("output/degradeRGB/degradeRGB.png");

    sil::Image imageVortex = vortex(image);
    imageVortex.save("output/vortex.png");

    sil::Image imageNorm = normalisation( photoFaible );
    imageNorm.save("output/normalisation.png");

    
    sil::Image photoTramage = tramage( photo );
    photoTramage.save("output/tramage.png");

    
    sil::Image imageConvo = convolution( image );
    /*
    for(int i{0}; i<100; i++){
         imageConvo = convolution( imageConvo );
    }
    */
    imageConvo.save("output/convo.png");
    

    //definition des differents noyaux de convolution
    std::vector<std::vector<float>> embossKernel {
        {-2.f, -1.f, 0.f},
        {-1.f,  1.f, 1.f},
        { 0.f,  1.f, 2.f}
    };

    
    std::vector<std::vector<float>> outlineKernel {
        {-1.f, -1.f, -1.f},
        {-1.f,  8.f, -1.f},
        {-1.f, -1.f, -1.f}
    };

   
    std::vector<std::vector<float>> sharpenKernel {
        { 0.f, -1.f,  0.f},
        {-1.f,  5.f, -1.f},
        { 0.f, -1.f,  0.f}
    };
    std::vector<std::vector<float>> baseKernel {
        { 1/9.f, 1/9.f,  1/9.f},
        {1/9.f,  1/9.f, 1/9.f},
        { 1/9.f, 1/9.f,  1/9.f}
    };

    // appel de la fonction
    sil::Image imageEmboss  = convolutionKernel(image, embossKernel);
    sil::Image imageOutline = convolutionKernel(image, outlineKernel);
    sil::Image imageSharpen = convolutionKernel(image, sharpenKernel);

    imageEmboss.save("output/emboss.png");
    imageOutline.save("output/outline.png");
    imageSharpen.save("output/sharpen.png");

    sil::Image imageConvoBase = convolutionKernel( image , baseKernel);
    imageConvoBase.save("output/convoBase.png");

    sil::Image imageKuwahara = kuwahara( photo );
    imageKuwahara.save("output/kuwahara.png");

    sil::Image diamond = diamondSquare( );
    diamond.save("output/diamond/diamond.png");

    sil::Image diamondColor = diamondSquareColor( );
    diamondColor.save("output/diamond/diamondColor.png");
}