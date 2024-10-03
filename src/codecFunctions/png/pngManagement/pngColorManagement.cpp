#include "pngColorManagement.hpp"

/// Standard libraries
#include <iostream>

/// Other files
#include "pixelUtils.hpp"

int getBytesPerPixelPNG(unsigned char colorType){
    switch(colorType){
        case 0: // Grayscale
            return 1;
        case 2: // RGB
            return 3;
        case 5: // Grayscale with alpha values (transparency)
            return 2;
        case 6: // RGB with alpha values (transparency)
            return 4;
    }

    std::cerr << "The color type of the image is not currently supported by the program\n";
    return -1;
}

int obtainGetPixelFunctionPNG(unsigned char colorType, 
                            std::function<Pixel(std::unique_ptr<unsigned char[]> &, int)> & getPixelFunction){
    switch(colorType){
        case 0: // Grayscale
            getPixelFunction = grayToPixel;
            break;
        case 2: // RGB
            getPixelFunction = rgbToPixel;
            break;
        case 5: // Grayscale with alpha values (transparency)
            getPixelFunction = grayWithAlphaToPixel;
            break;
        case 6: // RGB with alpha values (transparency)
            getPixelFunction = rgbWithAlphaToPixel;
            break;
        default:
            std::cerr << "The color type of the image is not currently supported by the program\n";
            return -1;
    }

    return 0;
}