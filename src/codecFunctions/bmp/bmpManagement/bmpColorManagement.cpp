#include "bmpColorManagement.hpp"

#include <iostream>

int bmpColorTypeToBytesPerPixel(ColorType colorType){

    switch (colorType){
        case GRAY:
        case GRAY_WITH_ALPHA: // BMP does not support gray with tranparency so it defaults to gray
            return 1;
            break;
        case COLOR:
            return 3;
        case COLOR_WITH_ALPHA:
            return 4;
        default:
            std::cerr << "Unknown color type, the output bmp will default to 3 bytes per Pixel\n";
            return 3;
            break;
    }
}