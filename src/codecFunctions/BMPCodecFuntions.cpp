#include <iostream>
#include <fstream>
#include "BMPCodecFunctions.h"
#include <memory>
#include <functional>

#include <math.h>

///// CONSTANTS /////

#define BITMAP_SIGNATURE 19778 // 19778 is 42 4D in hexadecimal, or BR in ASCII
#define FILE_HEADER_SIZE 14
#define INFO_HEADER_V5_SIZE 124

// Color masks
#define COLOR_MASK_FIRST_BYTE 255         // Represents the mask FF 00 00 00
#define COLOR_MASK_SECOND_BYTE 65280      // Represents the mask 00 FF 00 00
#define COLOR_MASK_THIRD_BYTE 16711680    // Represents the mask 00 00 FF 00
#define COLOR_MASK_FOURTH_BYTE 4278190080 // Represents the mask 00 00 00 FF


#define BGRs_INT_VALUE 1934772034 // 1934772034 is 42 47 52 73 in hexadecimal, or BGRs in ASCII


///// ENCODER /////

void add24bitPixelToRawImageData(Pixel pixel, std::unique_ptr<unsigned char[]> & rawImageData, int offSet){
    rawImageData[offSet] = pixel.blue;
    rawImageData[offSet+1] = pixel.green;
    rawImageData[offSet+2] = pixel.red;
}

int encodeBMP(std::fstream & outputFile, Image & image){

    //// Suport variables

    int bytesPerPixel = 3; //The 3 here represents the bytes per pixel of a 24 BMP image 
    int fillerBytes = (4 - (image.width*bytesPerPixel)%4)%4; 
    int rowSize = image.width*bytesPerPixel+fillerBytes;

    std::unique_ptr<unsigned char[]> rawImageData;

    //// File Header variables

    unsigned short signature = BITMAP_SIGNATURE;
    unsigned int totalLenght = FILE_HEADER_SIZE + INFO_HEADER_V5_SIZE + rowSize*image.heigth;
    unsigned int reserved = 0;
    unsigned int dataOffset = FILE_HEADER_SIZE + INFO_HEADER_V5_SIZE; // Offset that gets you where the pixel data is stored
    
    //// Info Header variables

    unsigned int infoHeaderSize = INFO_HEADER_V5_SIZE;
    unsigned int width = image.width;
    unsigned int height = image.heigth;
    unsigned short colorPlane = 1; // TODO Learn about color planes
    unsigned short bitsPerPixel = 24; // Until i add more options this function suports only 24 pixels
    unsigned int compressionMethod = 0; // TODO Learn about Bitmap compression methods
    unsigned int imageDataSize = height*rowSize;

    unsigned int verticalResolution = 0; // Prefered resolution, 0 equals no preference
    unsigned int horizontalResolution = 0; // Prefered resolution, 0 equals no preference
    unsigned int colorsUsed = 0; // We are not using fewer than the maximun number of colors so 0
    unsigned int numberOfImportantColors = 0; // Every color is taken as important so 0

    // Color masks for 24bits BGR bitmap
    unsigned int redMask = COLOR_MASK_THIRD_BYTE;
    unsigned int greenMask = COLOR_MASK_SECOND_BYTE;
    unsigned int blueMask = COLOR_MASK_THIRD_BYTE;
    unsigned int alphaMask = 0;

    unsigned int csType = BGRs_INT_VALUE;

    // cieXYZ Triple
    // Currently all 0 since this doesn't currently support the LCS_CALIBRATED_RGB format

    unsigned int ciexyzRedX = 0;
    unsigned int ciexyzRedY = 0;
    unsigned int ciexyzRedZ = 0;

    unsigned int ciexyzGreenX = 0;
    unsigned int ciexyzGreenY = 0;
    unsigned int ciexyzGreenZ = 0;

    unsigned int ciexyzBlueX = 0;
    unsigned int ciexyzBlueY = 0;
    unsigned int ciexyzBlueZ = 0;

    // Gamma

    unsigned int gammaRed = 0;
    unsigned int gammaGreen = 0;
    unsigned int gammaBlue = 0;

    // Other Header variables

    unsigned int rendenringIntent = 2; // TODO Learn more about this, for now 2
    unsigned int profileData = 0;
    unsigned int profileSize = 0;
    unsigned int infoHeaderReservedBytes = 0;

    //// If there is extra logic for the initialization of the variables it should go here

    //// Add the header in the file

    outputFile.write(reinterpret_cast<char*>(&signature), sizeof(unsigned short));
    outputFile.write(reinterpret_cast<char*>(&totalLenght), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&reserved), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&dataOffset), sizeof(unsigned int));
    
    outputFile.write(reinterpret_cast<char*>(&infoHeaderSize), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&width), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&height), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&colorPlane), sizeof(unsigned short));
    outputFile.write(reinterpret_cast<char*>(&bitsPerPixel), sizeof(unsigned short));
    outputFile.write(reinterpret_cast<char*>(&compressionMethod), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&imageDataSize), sizeof(unsigned int));

    outputFile.write(reinterpret_cast<char*>(&verticalResolution), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&horizontalResolution), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&colorsUsed), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&numberOfImportantColors), sizeof(unsigned int));

    outputFile.write(reinterpret_cast<char*>(&redMask), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&greenMask), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&blueMask), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&alphaMask), sizeof(unsigned int));

    outputFile.write(reinterpret_cast<char*>(&csType), sizeof(unsigned int));

    outputFile.write(reinterpret_cast<char*>(&ciexyzRedX), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&ciexyzRedY), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&ciexyzRedZ), sizeof(unsigned int));

    outputFile.write(reinterpret_cast<char*>(&ciexyzGreenX), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&ciexyzGreenY), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&ciexyzGreenZ), sizeof(unsigned int));

    outputFile.write(reinterpret_cast<char*>(&ciexyzBlueX), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&ciexyzBlueY), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&ciexyzBlueZ), sizeof(unsigned int));

    outputFile.write(reinterpret_cast<char*>(&gammaRed), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&gammaBlue), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&gammaGreen), sizeof(unsigned int));

    outputFile.write(reinterpret_cast<char*>(&rendenringIntent), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&profileData), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&profileSize), sizeof(unsigned int));
    outputFile.write(reinterpret_cast<char*>(&infoHeaderReservedBytes), sizeof(unsigned int));

    //// Image Data

    rawImageData = std::make_unique<unsigned char[]>(imageDataSize);

    for(int i = 0; i<height; ++i){

        for(int j = 0; j<width; ++j){
            add24bitPixelToRawImageData(image.imageData[(height-i-1)*width + j], rawImageData, i*rowSize + j*bytesPerPixel);     
        }

    }

    outputFile.write(reinterpret_cast<char*>(rawImageData.get()), imageDataSize);

    return 0;
}



///// DECODER /////

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

std::unique_ptr<Pixel[]> decodeColorImgData(unsigned int width, unsigned int height, 
                std::unique_ptr<unsigned char[]> & rawImageData, unsigned int bitsPerPixel){

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
            break;
        case 32:
            getPixelFunction = get32BitsPixel;
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


std::unique_ptr<Pixel[]> decodeGrayImageData(unsigned int width, unsigned int height, 
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

int decodeBMP(std::fstream & inputFile, Image & decodedImage){

    unsigned short signature;
    unsigned int lenght, dataOffset;

    unsigned int infoHeaderSize, width, height;
    unsigned short colorPlane, bitsPerPixel;
    unsigned int compressionMethod, dataSize; 
    unsigned int verticalResolution, horizontalResolution, numOfColors, numOfImportantColors;

    std::unique_ptr<unsigned char[]> rawImageData; // Stores the bytes of the data block of the bitmap

    std::unique_ptr<Pixel[]> imagePixels; // Stores the image data once parsed to Pixels

    //Image decodedImage;

    ///// FILE HEADER

    inputFile.read(reinterpret_cast<char*>(&signature), sizeof(unsigned short));

    if(signature != BITMAP_SIGNATURE){ 
        std::cout << "Not a Bitmap";
        return -1;
    }

    inputFile.read(reinterpret_cast<char*>(&lenght), sizeof(unsigned int));

    inputFile.seekg(10); // We move the data stream to avoid the reserved bytes

    inputFile.read(reinterpret_cast<char*>(&dataOffset), sizeof(unsigned int));

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


    inputFile.seekg(dataOffset);
    
    ///// PIXEL DATA

    rawImageData = std::make_unique<unsigned char[]>(dataSize);

    // Extract the data from the file
    inputFile.read(reinterpret_cast<char*>(rawImageData.get()), dataSize);


    // Parse the data into the pixel format
    switch(bitsPerPixel){
        case 1:
        case 2:
        case 4:
        case 8:
            imagePixels = decodeGrayImageData(width,height, rawImageData, bitsPerPixel);
            break;
        case 24:
        case 32:
            imagePixels = decodeColorImgData(width, height, rawImageData, bitsPerPixel);
            break;
        default:
            std::cout << "The program has no support to bitMaps with " << bitsPerPixel << " bitsPerPixels.\n";
            return -1;
    }


    decodedImage.heigth = height;
    decodedImage.width = width;
    decodedImage.imageData = move(imagePixels);

    return 0;
}