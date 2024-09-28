#include <iostream>

#include "pngCodecFunctions.hpp"
#include "pngChunkManagement.hpp"
#include "fileDataManagementUtils.hpp"

#define PNG_SIGNATURE 727905341920923785 



int decodePNG(std::fstream & inputFile, Image & decodedImage){

    /// Signature
    unsigned long long signature;

    /// Header chunk variables
    unsigned int width;
    unsigned int height;
    unsigned char bitDepth;
    unsigned char colorType;
    unsigned char filterMethod;
    unsigned char compressMethod;
    unsigned char interlacing;

    /// Chunk Management variables
    unsigned int chunkLenght;
    unsigned int chunkName;
    std::unique_ptr<unsigned char[]> rawChunkData;

    /// Result variables
    std::unique_ptr<Pixel[]> imagePixels;

    /// Support variables
    bool iendChunkReached = false;

    //// Signature management ////

    inputFile.read(reinterpret_cast<char*>(&signature), sizeof(unsigned long long));

    if(signature != PNG_SIGNATURE){
        std::cout << "Not a PNG, the signature does not match\n";
        return -1;
    }

    if(getChunk(inputFile, chunkLenght, chunkName, rawChunkData)){
        return -2;
    }

    if(chunkName != IHDR){
        std::cout << "The input png file does not have a header chunk\n";
        return -3;
    }

    width = extractBigEndianUInt(rawChunkData, 0);
    height = extractBigEndianUInt(rawChunkData, 4);

    bitDepth = rawChunkData[8];
    colorType = rawChunkData[9];
    filterMethod = rawChunkData[10];
    compressMethod = rawChunkData[11];
    interlacing = rawChunkData[12];


    imagePixels = std::make_unique<Pixel[]>(height*width);

    while(!iendChunkReached){
        
        if(getChunk(inputFile, chunkLenght, chunkName, rawChunkData)){
            return -2;
        }


        switch (chunkName){
            case IDAT:
                
                break;
            case IEND:
                iendChunkReached = true;
                break;
        }


    }

    //crc(rawChunkData.get(), chunkLenght);

    //std::cout << chunkLenght << "\n";

    return 0;
}