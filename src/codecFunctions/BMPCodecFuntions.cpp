#include <iostream>
#include <fstream>
#include "BMPCodecFunctions.h"
#include <memory>

#include <math.h>

std::unique_ptr<Pixel[]> decodeImgData32Bits(unsigned int width, unsigned int height, std::unique_ptr<unsigned char[]> & rawImageData){

    std::unique_ptr<Pixel[]> imagePixels = std::make_unique<Pixel[]>(height*width);

    Pixel nPixel;

    for(int i = 0; i<height; ++i){
        // BMP stores the pixels in rows

        // Although the compiler would probably do this optimization, its ok doing it for the sake of readability

        // The offset that determines the row you are in, 
        // we multiply width by 4 because each pixel ocupies 4 bytes in the binary.
        int heightOffSet = i*width*4;

        for(int j = 0; j<width; ++j){
            nPixel.blue = rawImageData[heightOffSet + j*4];
            nPixel.green = rawImageData[heightOffSet + j*4 + 1];
            nPixel.red = rawImageData[heightOffSet + j*4 + 2];
            nPixel.alpha = rawImageData[heightOffSet + j*4 + 3];

            // We use height-i-1 because the data is stored from botton to top, and the imageData
            // in the Image struct should be stored from top to bottom.
            imagePixels[(height-i-1)*width+j] = nPixel; 
        }
    }

    return imagePixels;
}

std::unique_ptr<Pixel[]> decodeImgData24Bits(unsigned int width, unsigned int height, std::unique_ptr<unsigned char[]> & rawImageData){

    std::unique_ptr<Pixel[]> imagePixels = std::make_unique<Pixel[]>(height*width);
    
    // When the row ends, the BMP format fills the space with 0s until the next multiple of 4
    // So we have to skip those bytes to get to the next row.
    int fillerPixels = (4 - width%4)%4;

    Pixel nPixel;
    nPixel.alpha = 255; // 24 bits Bitmap don't store alpha bits, that will be translated as the maximun alpha value (fully opaque)

    for(int i = 0; i<height; ++i){
        // BMP stores the pixels in rows

        // Although the compiler would probably do this optimization, its ok doing it for the sake of readability

        // The offset that determines the row you are in, 
        // we multiply width by 3 because each pixel ocupies 3 bytes in the binary.
        int heightOffSet = i*(width*3+fillerPixels);

        for(int j = 0; j<width; ++j){
            nPixel.blue = rawImageData[heightOffSet + j*3];
            nPixel.green = rawImageData[heightOffSet + j*3 + 1];
            nPixel.red = rawImageData[heightOffSet + j*3 + 2];

            // We use height-i-1 because the data is stored from botton to top, and the imageData
            // in the Image struct should be stored from top to bottom.
            imagePixels[(height-i-1)*width+j] = nPixel; 
        }
    }


    return imagePixels;

}


std::unique_ptr<Pixel[]> decodeGrayImageData(unsigned int width, unsigned int height, 
                std::unique_ptr<unsigned char[]> & rawImageData, unsigned int bitsPerPixel){
    
    std::unique_ptr<Pixel[]> imagePixels = std::make_unique<Pixel[]>(height*width);
    
    Pixel nPixel;
    nPixel.alpha = 255; // Gray Bitmaps don't store alpha bits, that will be translated as the maximun alpha value (fully opaque)

    int pixelsPerByte = 9-bitsPerPixel;

    int magicNumber = width*bitsPerPixel/8;
    if(width*bitsPerPixel%8){
        ++magicNumber;
    }

    int fillerPixels = (4 - magicNumber%4)%4;

    int statingBits = std::pow(2,8-bitsPerPixel);
    int inverseBits = 256/statingBits;

    for(int i = 0; i<height; ++i){
        // BMP stores the pixels in rows

        int heightOffSet = i*(magicNumber+fillerPixels);

        for(int j = 0; j<magicNumber; ++j){
            
            char nByteVal = rawImageData[heightOffSet + j];

            for(int k = 0, bitMult = statingBits; bitMult>0; ++k, bitMult = bitMult/inverseBits){
                int greyVal = nByteVal/bitMult;
                nByteVal =- greyVal*bitMult;
                
                int standarizedVal = (greyVal+1)*statingBits -1;
                nPixel.blue = standarizedVal;
                nPixel.green = standarizedVal;
                nPixel.red = standarizedVal;
                
                imagePixels[(height-i-1)*width+pixelsPerByte*j+k] = nPixel; 
            }
        }
    }

    return imagePixels;
}

int decodeBMP(std::fstream & inputFile, Image & decodedImage){

    unsigned short signature;
    unsigned int lenght, dataPos;

    unsigned int infoHeaderSize, width, height;
    unsigned short colorPlane, bitsPerPixel;
    unsigned int compressionMethod, dataSize; 
    unsigned int verticalResolution, horizontalResolution, numOfColors, numOfImportantColors;

    std::unique_ptr<unsigned char[]> rawImageData; // Stores the bytes of the data block of the bitmap

    std::unique_ptr<Pixel[]> imagePixels; // Stores the image data once parsed to Pixels

    //Image decodedImage;

    ///// FILE HEADER

    inputFile.read(reinterpret_cast<char*>(&signature), sizeof(unsigned short));

    if(signature != 19778){ // 19778 is 42 4D in hexadecimal, or BR in ASCII
        std::cout << "Not a Bitmap";
        return -1;
    }

    inputFile.read(reinterpret_cast<char*>(&lenght), sizeof(unsigned int));

    inputFile.seekg(10); // We move the data stream to avoid the reserved bytes

    inputFile.read(reinterpret_cast<char*>(&dataPos), sizeof(unsigned int));

    ///// INFO HEADER

    inputFile.read(reinterpret_cast<char*>(&infoHeaderSize), sizeof(unsigned int));

    inputFile.read(reinterpret_cast<char*>(&width), sizeof(unsigned int));
    inputFile.read(reinterpret_cast<char*>(&height), sizeof(unsigned int));

    inputFile.read(reinterpret_cast<char*>(&colorPlane), sizeof(unsigned short));
    inputFile.read(reinterpret_cast<char*>(&bitsPerPixel), sizeof(unsigned short));

    inputFile.read(reinterpret_cast<char*>(&compressionMethod), sizeof(unsigned int));
    inputFile.read(reinterpret_cast<char*>(&dataSize), sizeof(unsigned int));

    inputFile.read(reinterpret_cast<char*>(&verticalResolution), sizeof(unsigned int));
    inputFile.read(reinterpret_cast<char*>(&horizontalResolution), sizeof(unsigned int));
    inputFile.read(reinterpret_cast<char*>(&numOfColors), sizeof(unsigned int));
    inputFile.read(reinterpret_cast<char*>(&numOfImportantColors), sizeof(unsigned int));


    inputFile.seekg(dataPos);
    
    ///// PIXEL DATA

    rawImageData = std::make_unique<unsigned char[]>(dataSize);

    // Extract the data from the file
    for(int i = 0; i<dataSize; ++i){
        rawImageData[i] = inputFile.get();
    }


    // Parse the data into the pixel format
    switch(bitsPerPixel){
        case 1:
            imagePixels = decodeGrayImageData(width,height, rawImageData, bitsPerPixel);
            break;
        case 2:
            imagePixels = decodeGrayImageData(width,height, rawImageData, bitsPerPixel);
            break;
        case 4:
            imagePixels = decodeGrayImageData(width,height, rawImageData, bitsPerPixel);
            break;
        case 8:
            imagePixels = decodeGrayImageData(width,height, rawImageData, bitsPerPixel);
            break;
        case 24:
            imagePixels = decodeImgData24Bits(width, height, rawImageData);
            break;
        case 32:
            imagePixels = decodeImgData32Bits(width, height, rawImageData);
            break;
        default:
            std::cout << "The program has no support to bitMaps with " << bitsPerPixel << " bitsPerPixels.\n";
            return -1;
    }

    // Small Test
    // for(int i = 0; i<height*width; ++i){
    //     std::cout << imageData[i].blue-0 << " " << imageData[i].green-0 << " " << imageData[i].red-0 << "\n";
    // }

    decodedImage.heigth = height;
    decodedImage.width = width;
    decodedImage.imageData = move(imagePixels);

    return 0;
}