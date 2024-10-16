#include "pngColorManagement.hpp"

/// Standard libraries
#include <iostream>


int getBytesPerPixelPNG(unsigned char colorType){
    switch(colorType){
        case 0: // Grayscale
            return 1;
        case 2: // RGB
            return 3;
        case 4: // Grayscale with alpha values (transparency)
            return 2;
        case 6: // RGB with alpha values (transparency)
            return 4;
    }

    std::cerr << "The color type of the image is not currently supported by the program\n";
    return -1;
}

unsigned char getCharColorTypePNG(ColorType enumColorType){
    switch(enumColorType){
        case GRAY: // Grayscale
            return 0;
        case COLOR: // RGB
            return 2;
        case GRAY_WITH_ALPHA: // Grayscale with alpha values (transparency)
            return 4;
        case COLOR_WITH_ALPHA: // RGB with alpha values (transparency)
            return 6;
    }

    std::cerr << "The color type or transparency of the image is not currently supported by the program for encoding PNGs\n";
    return UNDEFINED_COLOR;
}

ColorType getEnumColorTypePNG(unsigned char charColorType){
    switch(charColorType){
        case 0: // Grayscale
            return GRAY;
        case 2: // RGB
            return COLOR;
        case 4: // Grayscale with alpha values (transparency)
            return GRAY_WITH_ALPHA;
        case 6: // RGB with alpha values (transparency)
            return COLOR_WITH_ALPHA;
    }

    std::cerr << "The color type of the image is not currently supported by the program\n";
    return UNDEFINED_COLOR;
}