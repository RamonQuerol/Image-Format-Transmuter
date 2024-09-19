#include <iostream>
#include <fstream>
#include "BMPCodecFunctions.h"


Pixel decodePixelBMP(std::fstream *inputFile){
    Pixel pixel;
    pixel.blue = inputFile->get();
    pixel.green = inputFile->get();
    pixel.red = inputFile->get();
    pixel.alpha = 255; // BMP as far as i know doesn't use alpha values, that will be translated as the maximun alpha value (fully opaque)

    return pixel;
}

int decodeBMP(std::fstream *inputFile, Image *decodedImage){

    unsigned short signature;
    unsigned int lenght, dataPos;

    unsigned int infoHeaderSize, width, height;
    unsigned short colorPlane, bitsPerPixel;
    unsigned int compressionMethod, dataSize; 
    unsigned int verticalResolution, horizontalResolution, numOfColors, numOfImportantColors;

    
    //Image decodedImage;

    ///// FILE HEADER

    inputFile->read(reinterpret_cast<char*>(&signature), sizeof(unsigned short));

    if(signature != 19778){ // 19778 is 42 4D in hexadecimal, or BR in ASCII
        std::cout << "Not a Bitmap";
        return -1;
    }

    inputFile->read(reinterpret_cast<char*>(&lenght), sizeof(unsigned int));

    inputFile->seekg(10); // We move the data stream to avoid the reserved bytes

    inputFile->read(reinterpret_cast<char*>(&dataPos), sizeof(unsigned int));

    ///// INFO HEADER

    inputFile->read(reinterpret_cast<char*>(&infoHeaderSize), sizeof(unsigned int));

    inputFile->read(reinterpret_cast<char*>(&width), sizeof(unsigned int));
    inputFile->read(reinterpret_cast<char*>(&height), sizeof(unsigned int));

    inputFile->read(reinterpret_cast<char*>(&colorPlane), sizeof(unsigned short));
    inputFile->read(reinterpret_cast<char*>(&bitsPerPixel), sizeof(unsigned short));

    inputFile->read(reinterpret_cast<char*>(&compressionMethod), sizeof(unsigned int));
    inputFile->read(reinterpret_cast<char*>(&dataSize), sizeof(unsigned int));

    inputFile->read(reinterpret_cast<char*>(&verticalResolution), sizeof(unsigned int));
    inputFile->read(reinterpret_cast<char*>(&horizontalResolution), sizeof(unsigned int));
    inputFile->read(reinterpret_cast<char*>(&numOfColors), sizeof(unsigned int));
    inputFile->read(reinterpret_cast<char*>(&numOfImportantColors), sizeof(unsigned int));


    inputFile->seekg(dataPos);

    Pixel *imageData = new Pixel[height*width];
    
    for(int i = 0; i<height; ++i){
        // BMP stores the pixels in rows

        for(int j = 0; j<width; ++j){
            // We use height-i-1 because the data is stored from botton to top, and the imageData
            // in the Image struct should be stored from top to bottom.
            imageData[(height-i-1)*width+j] = decodePixelBMP(inputFile); 
        }
        
        // When the row ends, the BMP format fills the space with 0s until the next multiple of 4
        // So we have to skip those bytes to get to the next row.
        inputFile->seekg(width%4, std::ios::cur);
    }

    // Small Test
    // for(int i = 0; i<height*width; ++i){
    //     cout << imageData[i].blue-0 << " " << imageData[i].green-0 << " " << imageData[i].red-0 << "\n";
    // }

    decodedImage->heigth = height;
    decodedImage->width = width;
    decodedImage->imageData = imageData;

    return 0;
}