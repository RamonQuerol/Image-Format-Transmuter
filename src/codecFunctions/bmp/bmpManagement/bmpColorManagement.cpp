#include "bmpColorManagement.hpp"

#include <iostream>
#include <memory>
#include <cstring>



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


void bmpAdd8BitsColorTable(std::fstream & outputFile){

    std::unique_ptr<unsigned char[]> colorTable = std::make_unique<unsigned char[]>(GRAY_8BIT_COLOR_TABLE_SIZE);
    int colorTableOffset = 0;

    for(int i = 0; i<GRAY_8BIT_COLOR_TABLE_SIZE/4; ++i, colorTableOffset += 4){
        memset(colorTable.get()+colorTableOffset, i, 3);
    }

    outputFile.write(reinterpret_cast<char*>(colorTable.get()), GRAY_8BIT_COLOR_TABLE_SIZE);
}