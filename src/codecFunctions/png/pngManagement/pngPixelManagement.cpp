#include "pngPixelManagement.hpp"

#include <memory>
#include <iostream>
#include <cstring>

#include "dataStructures.hpp"


///// Prototype functions 

/// Pixel array to byte array

// Each of this functions loops through imagePixels, transforms the Pixel struct into bytes
// and stores those bytes inside rawImageData.
//
// The difference between each function is the kind of image they are copying (colorType)

void pixelToGray(std::unique_ptr<Pixel[]> & imagePixels, int numPixels,
                    std::unique_ptr<unsigned char[]> & rawImageData);

void pixelToGrayWithAlpha(std::unique_ptr<Pixel[]> & imagePixels, int numPixels,
                            std::unique_ptr<unsigned char[]> & rawImageData);

void pixelToRgb(std::unique_ptr<Pixel[]> & imagePixels, int numPixels,
                    std::unique_ptr<unsigned char[]> & rawImageData);

void pixelToRgbWithAlpha(std::unique_ptr<Pixel[]> & imagePixels, int numPixels,
                            std::unique_ptr<unsigned char[]> & rawImageData);


/// Byte array to Pixel array

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

// TODO Test the function and all its support functions (can't do it until i have the png encoder)
int translateToByteArray(std::unique_ptr<Pixel[]> imagePixels, ColorType colorType,
                            int numPixels, std::unique_ptr<unsigned char[]> & rawPixelData){

        switch(colorType){
        case GRAY: // Grayscale
            pixelToGray(imagePixels, numPixels, rawPixelData);
            break;
        case COLOR: // RGB
            pixelToRgb(imagePixels, numPixels, rawPixelData);
            break;
        case GRAY_WITH_ALPHA: // Grayscale with alpha values (transparency)
            pixelToGrayWithAlpha(imagePixels, numPixels, rawPixelData);
            break;
        case COLOR_WITH_ALPHA: // RGB with alpha values (transparency)
            pixelToRgbWithAlpha(imagePixels, numPixels, rawPixelData);
            break;
        default:
            std::cerr << "The color type of the image is not currently supported by the program\n";
            return -1;
    }

    return 0;
}



int translateToPixelArray(std::unique_ptr<unsigned char[]> rawPixelData, unsigned char colorType,
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


/// Pixel array to byte array

void pixelToGray(std::unique_ptr<Pixel[]> & imagePixels, int numPixels,
                    std::unique_ptr<unsigned char[]> & rawImageData){

    for(int i = 0; i<numPixels; ++i){
        rawImageData[i] = imagePixels[i].red;
    }
}

void pixelToGrayWithAlpha(std::unique_ptr<Pixel[]> & imagePixels, int numPixels,
                            std::unique_ptr<unsigned char[]> & rawImageData){

    /////// IMPORTANT //////
    // This pointers must not under circumstances be freed
    // The only reason we are using them, is to use memset so the program runs faster
    unsigned char *imagePixelsPointer = reinterpret_cast<unsigned char *>(imagePixels.get()); // We reinterpret this move through memory 1 byte at a time when adding (Instead of 4 that is the size of a Pixel)
    unsigned char *rawImageDataPointer = rawImageData.get();

    int rawImOffset = 0;
    int imPixelsOffset = 2;

    for(int i = 0; i<numPixels; ++i, rawImOffset = 2, imPixelsOffset += 4){
        memcpy(rawImageDataPointer+rawImOffset, imagePixelsPointer+imPixelsOffset, 2);
    }
    
}

void pixelToRgb(std::unique_ptr<Pixel[]> & imagePixels, int numPixels,
                    std::unique_ptr<unsigned char[]> & rawImageData){
    
    int rawImOffSet = 0;

    /////// IMPORTANT //////
    // This pointers must not under circumstances be freed
    // The only reason we are using them, is to use memset and memcpy so the program runs faster
    Pixel *imagePixelsPointer = imagePixels.get();
    unsigned char *rawImageDataPointer = rawImageData.get();

    for(int i = 0; i<numPixels; ++i, rawImOffSet += 3){

        // This is an efficient way of doing this: 
        // imagePixels[i].red = rawImageData[offSet];
        // imagePixels[i].green = rawImageData[offSet + 1];
        // imagePixels[i].blue = rawImageData[offSet + 2];
        memcpy(rawImageDataPointer+rawImOffSet, imagePixelsPointer+i,  3);
    }

}

void pixelToRgbWithAlpha(std::unique_ptr<Pixel[]> & imagePixels, int numPixels,
                            std::unique_ptr<unsigned char[]> & rawImageData){
    
    // Since Pixel is a struct with the order red, green, blue and alpha, the order in memory
    // is the same as the one in rawImageData. So we can just copy the memory from one array to the other

    memcpy(rawImageData.get(), imagePixels.get(), numPixels*4);

}



/// Byte array to Pixel array


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
        // imagePixels[i].red = rawImageData[offSet];
        // imagePixels[i].green = rawImageData[offSet];
        // imagePixels[i].blue = rawImageData[offSet];
        memset(imagePixelsPointer+i, *(rawImageDataPointer+offSet), 3);
    }
}

void grayWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels){

    /////// IMPORTANT //////
    // This pointers must not under circumstances be freed
    // The only reason we are using them, is to use memset so the program runs faster
    unsigned char *imagePixelsPointer = reinterpret_cast<unsigned char *>(imagePixels.get()); // We reinterpret this move through memory 1 byte at a time when adding (Instead of 4 that is the size of a Pixel)
    unsigned char *rawImageDataPointer = rawImageData.get();

    int imPixelsOffset = 0;
    int rawImOffSet = 0;

    for(int i = 0; i<numPixels; ++i, imPixelsOffset +=4, rawImOffSet += 2){

        // In RGB gray values are obtained by having the same value in red, green and blue

        // This two function is an efficient way of doing this:
        // imagePixels[i].red = rawImageData[offSet];
        // imagePixels[i].green = rawImageData[offSet];
        // imagePixels[i].blue = rawImageData[offSet];
        // imagePixels[i].alpha = rawImageData[offSet+1];
        memset(imagePixelsPointer+imPixelsOffset, rawImageData[rawImOffSet], 2);

        memcpy(imagePixelsPointer+imPixelsOffset+2, rawImageDataPointer+rawImOffSet, 2);

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
        // imagePixels[i].red = rawImageData[offSet];
        // imagePixels[i].green = rawImageData[offSet + 1];
        // imagePixels[i].blue = rawImageData[offSet + 2];
        memcpy(imagePixelsPointer+i, rawImageDataPointer+offSet, 3);
    }
}


void rgbWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int numPixels,
                    std::unique_ptr<Pixel[]> & imagePixels){
    
    // Since Pixel is a struct with the order red, green, blue and alpha, the order in memory
    // is the same as the one in rawImageData. So we can just copy the memory from one array to the other

    memcpy(imagePixels.get(),rawImageData.get(), numPixels*4);
}