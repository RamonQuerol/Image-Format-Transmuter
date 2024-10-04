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