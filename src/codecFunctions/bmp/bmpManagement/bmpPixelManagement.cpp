#include "bmpPixelManagement.hpp"

#include <functional>
#include <math.h>
#include <cstring>

#include <iostream>

///// ENCODING /////

void pixelRowToGrayBMP(std::unique_ptr<Pixel[]> & imagePixels, unsigned int width,
                            unsigned int imgPixelsOffset,unsigned int rawDataOffset, 
                            std::unique_ptr<unsigned char[]> & rawImageData){

    for(int i = 0; i<width; ++i){
        rawImageData[rawDataOffset+i] = imagePixels[imgPixelsOffset+i].red;
    }
}


void pixelRowToColorBMP(std::unique_ptr<Pixel[]> & imagePixels, unsigned int width, unsigned int bytesPerPixel,
                            unsigned int imagePixelsInitOffset,unsigned int rawDataOffset, 
                            std::unique_ptr<unsigned char[]> & rawImageData){
    int rawDataPos = rawDataOffset;


    Pixel *imagePixelsPointer = imagePixels.get();
    unsigned char *rawDataPointer = rawImageData.get();

    for(int j = 0; j<width; ++j, rawDataPos += bytesPerPixel){

        std::memcpy(rawDataPointer + rawDataPos, imagePixelsPointer + imagePixelsInitOffset +j, bytesPerPixel);

        // We have to swap because BMP is in BGR and Pixel format in RGB
        std::swap(rawDataPointer[rawDataPos], rawDataPointer[rawDataPos+2]); 

    }
}

///// DECODING /////

Pixel get24BitsPixel(std::unique_ptr<unsigned char[]> & rawImageData, int offSet){
    Pixel pixel;

    pixel.blue = rawImageData[offSet];
    pixel.green = rawImageData[offSet + 1];
    pixel.red = rawImageData[offSet + 2];
    pixel.alpha = 255; // 24 bits Bitmap don't store alpha bits, that will be translated as the maximun alpha value (fully opaque)

    return pixel;
}


Pixel get32BitsPixel(std::unique_ptr<unsigned char[]> & rawImageData, int offSet){
    Pixel pixel;

    pixel.blue = rawImageData[offSet];
    pixel.green = rawImageData[offSet + 1];
    pixel.red = rawImageData[offSet + 2];
    pixel.alpha = rawImageData[offSet + 3];

    return pixel;
}

std::unique_ptr<Pixel[]> decodeColorImgDataBMP(unsigned int width, unsigned int height, 
                std::unique_ptr<unsigned char[]> & rawImageData, unsigned int bitsPerPixel,
                ColorType & colorType){

    std::unique_ptr<Pixel[]> imagePixels = std::make_unique<Pixel[]>(height*width);
    
    int bytesPerPixel = bitsPerPixel/8;

    // When the row ends, the BMP format fills the space with 0s until the next multiple of 4
    // So we have to skip those bytes to get to the next row.
    int fillerPixels = (4 - (width*bytesPerPixel)%4)%4; // The second %4 is there so if its 4 fillerPixels equals 0
    int bytesPerRow = width*bytesPerPixel+fillerPixels;

    Pixel nPixel;

    std::function<Pixel(std::unique_ptr<unsigned char[]> &, int)> getPixelFunction;

    switch(bitsPerPixel){
        case 24:
            getPixelFunction = get24BitsPixel;
            colorType = COLOR;
            break;
        case 32:
            getPixelFunction = get32BitsPixel;
            colorType = COLOR_WITH_ALPHA;
            break;
        // TODO Although nothing should get through here without a value, i should add a default case just in case
    }

    for(int i = 0; i<height; ++i){
        // BMP stores the pixels in rows

        // Although the compiler would probably do this optimization, its ok doing it for the sake of readability

        // The offset that determines the row you are in, 
        int heightOffSet = i*bytesPerRow;

        for(int j = 0; j<width; ++j){
            nPixel = getPixelFunction(rawImageData, heightOffSet + j*bytesPerPixel);

            // We use height-i-1 because the data is stored from botton to top, and the imageData
            // in the Image struct should be stored from top to bottom.
            imagePixels[(height-i-1)*width+j] = nPixel; 
        }
    }


    return imagePixels;

}


std::unique_ptr<Pixel[]> decodeGrayImageDataBMP(unsigned int width, unsigned int height, 
                std::unique_ptr<unsigned char[]> & rawImageData, unsigned int bitsPerPixel){
    
    std::unique_ptr<Pixel[]> imagePixels = std::make_unique<Pixel[]>(height*width);
    
    Pixel nPixel;
    nPixel.alpha = 255; // Gray Bitmaps don't store alpha bits, that will be translated as the maximun alpha value (fully opaque)

    int pixelsPerByte = 9-bitsPerPixel;

    int bytesPerRow = width*bitsPerPixel/8;
    if(width*bitsPerPixel%8){
        ++bytesPerRow;
    }

    // When the row ends, the BMP format fills the space with 0s until the next multiple of 4 byte
    // So we have to skip those bytes to get to the next row.
    int fillerPixels = (4 - bytesPerRow%4)%4;

    // Variables needed to extract the pixels from each byte
    int statingBitMult = std::pow(2,pixelsPerByte-1);
    int maxPixelValue = 256/statingBitMult;

    for(int i = 0; i<height; ++i){
        // BMP stores the pixels in rows

        int heightOffSet = i*(bytesPerRow+fillerPixels);

        // We iterate through all the bytes in the row
        for(int j = 0; j<bytesPerRow; ++j){
            
            unsigned char nByteVal = rawImageData[heightOffSet + j];

            // We decode all the pixels in the byte
            for(int k = 0, bitMult = statingBitMult; bitMult>0; ++k, bitMult = bitMult/maxPixelValue){
                
                //  We extract the pixel the byte
                int greyVal = nByteVal/bitMult;
                nByteVal =- greyVal*bitMult;
                
                // Since the value is in another base (for example 1 in 1bit BMP is 255 in 8bit) we need to translate the values to a standarized base (8bits)
                int standarizedVal = (greyVal+1)*statingBitMult -1;
                
                // In BGR/RBG images gray happens when all the values are the same.
                nPixel.blue = standarizedVal;
                nPixel.green = standarizedVal;
                nPixel.red = standarizedVal;
                
                imagePixels[(height-i-1)*width+pixelsPerByte*j+k] = nPixel; 
            }
        }
    }

    return imagePixels;
}