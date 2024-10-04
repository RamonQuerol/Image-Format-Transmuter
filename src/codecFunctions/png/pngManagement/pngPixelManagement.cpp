#include <memory>
#include <iostream>
#include <cstring>

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
        case 4: // Grayscale with alpha values (transparency)
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
    
    int offSet = 0;

    /////// IMPORTANT //////
    // This pointers must not under circumstances be freed
    // The only reason we are using them, is to use memset so the program runs faster
    Pixel *imagePixelsPointer = imagePixels.get();
    unsigned char *rawImageDataPointer = rawImageData.get();


    // The idea here is to set all the array to the maximun value so we can set all the alpha values
    // at the same time.
    // The alpha values are 255 because that means the image is fully opaque
    memset(imagePixelsPointer, 255, numPixels*4);

    for(int i = 0; i<numPixels; ++i, offSet += 1){

        // In RGB gray values are obtained by havinf the same value in red, green and blue
        // pixel.red = rawImageData[offSet];
        // pixel.green = rawImageData[offSet];
        // pixel.blue = rawImageData[offSet];
        memset(imagePixelsPointer+i, *(rawImageDataPointer+offSet), 3);
    }
}

void grayWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels){

    int offSet = 0;

    /////// IMPORTANT //////
    // This pointers must not under circumstances be freed
    // The only reason we are using them, is to use memset so the program runs faster
    Pixel *imagePixelsPointer = imagePixels.get();
    unsigned char *rawImageDataPointer = rawImageData.get();

    for(int i = 0; i<numPixels; ++i, offSet += 2){

        // In RGB gray values are obtained by having the same value in red, green and blue

        // This two function is an efficient way of doing this:
        // pixel.red = rawImageData[offSet];
        // pixel.green = rawImageData[offSet];
        // pixel.blue = rawImageData[offSet];
        //imagePixels[i].alpha = rawImageData[offSet+1];
        memset(imagePixelsPointer+i, rawImageData[offSet], 2);

        memcpy(imagePixelsPointer+i, rawImageDataPointer+offSet+1, 2);

    }
}

void rgbToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels){
    
    int offSet = 0;

    /////// IMPORTANT //////
    // This pointers must not under circumstances be freed
    // The only reason we are using them, is to use memset and memcpy so the program runs faster
    Pixel *imagePixelsPointer = imagePixels.get();
    unsigned char *rawImageDataPointer = rawImageData.get();

    // The idea here is to set all the array to the maximun value so we can set all the alpha values
    // at the same time.
    // The alpha values are 255 because that means the image is fully opaque
    memset(imagePixelsPointer, 255, numPixels*4);

    for(int i = 0; i<numPixels; ++i, offSet += 3){

        // This is an efficient way of doing this: 
        // pixel.red = rawImageData[offSet];
        // pixel.green = rawImageData[offSet + 1];
        // pixel.blue = rawImageData[offSet + 2];
        memcpy(imagePixelsPointer+i, rawImageDataPointer+offSet, 3);
    }
}


void rgbWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels){
    
    // Since Pixel is a struct with the order red, green, blue and alpha, the order in memory
    // is the same as the one in rawImageData. So we can just copy the memory from one array to the other

    memcpy(imagePixels.get(),rawImageData.get(), numPixels*4);
}