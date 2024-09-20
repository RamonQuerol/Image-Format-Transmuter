#include <iostream>
#include <fstream>
#include "BMPCodecFunctions.h"
#include <memory>

std::unique_ptr<Pixel[]> decodeImgData24Bits(unsigned int width, unsigned int height, std::unique_ptr<unsigned char[]> & rawImageData){

    std::unique_ptr<Pixel[]> imagePixels = std::make_unique<Pixel[]>(height*width);
    
    // When the row ends, the BMP format fills the space with 0s until the next multiple of 4
    // So we have to skip those bytes to get to the next row.
    int fillerPixels = width%4;

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
        case 24:
            imagePixels = decodeImgData24Bits(width, height, rawImageData);
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