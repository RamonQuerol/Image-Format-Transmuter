#include <memory>
#include <iostream>

#include "dataStructures.hpp"


///// Prototype functions 

// Each of this functions loops through rawImageData, transforms their data into Pixels
// and stores those pixels inside imagePixels.
//
// The difference between each function is the kind of image they are copying (colorType)

void grayToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels);

void grayWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels);

void rgbToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels);

void rgbWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels);


////// Main Functions

int translateToPixelArray(std::unique_ptr<unsigned char[]> & rawPixelData, unsigned char colorType,
                            int numPixels, std::unique_ptr<Pixel[]> & imagePixels){

        switch(colorType){
        case 0: // Grayscale
            grayToPixel(rawPixelData, numPixels, imagePixels);
            break;
        case 2: // RGB
            rgbToPixel(rawPixelData, numPixels, imagePixels);
            break;
        case 5: // Grayscale with alpha values (transparency)
            grayWithAlphaToPixel(rawPixelData, numPixels, imagePixels);
            break;
        case 6: // RGB with alpha values (transparency)
            rgbWithAlphaToPixel(rawPixelData, numPixels, imagePixels);
            break;
        default:
            std::cerr << "The color type of the image is not currently supported by the program\n";
            return -1;
    }

    return 0;
}



///// Suport functions

void grayToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels){
    
    Pixel pixel;
    int offSet = 0;

    for(int i = 0; i<numPixels; ++i, offSet += 1){

        // In RGB gray values are obtained by havinf the same value in red, green and blue
        pixel.red = rawImageData[offSet];
        pixel.green = rawImageData[offSet];
        pixel.blue = rawImageData[offSet];
        pixel.alpha = 255; // Since we don't have alpha, we will put maximun alpha value (fully opaque)
    
        imagePixels[i] = pixel;
    }
}

void grayWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels){

    Pixel pixel;
    int offSet = 0;

    for(int i = 0; i<numPixels; ++i, offSet += 2){

        // In RGB gray values are obtained by havinf the same value in red, green and blue
        pixel.red = rawImageData[offSet];
        pixel.green = rawImageData[offSet];
        pixel.blue = rawImageData[offSet];

        pixel.alpha = rawImageData[offSet+1];

        imagePixels[i] = pixel;
    }
}

void rgbToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels){
    
    Pixel pixel;
    int offSet = 0;

    for(int i = 0; i<numPixels; ++i, offSet += 3){

        pixel.red = rawImageData[offSet];
        pixel.green = rawImageData[offSet + 1];
        pixel.blue = rawImageData[offSet + 2];
        pixel.alpha = 255; // Since we don't have alpha, we will put maximun alpha value (fully opaque)

        imagePixels[i] = pixel;
    }
}


void rgbWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels){
    
    Pixel pixel;
    int offSet = 0;

    for(int i = 0; i<numPixels; ++i, offSet += 4){

        pixel.red = rawImageData[offSet];
        pixel.green = rawImageData[offSet + 1];
        pixel.blue = rawImageData[offSet + 2];
        pixel.alpha = rawImageData[offSet + 3];

        imagePixels[i] = pixel;
    }
}