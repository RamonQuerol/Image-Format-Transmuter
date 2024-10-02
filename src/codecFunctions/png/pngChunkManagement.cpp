

#include "pngChunkManagement.hpp"
#include "fileDataManagementUtils.hpp"
#include "pngCRC.hpp"
#include "pixelUtils.hpp"

int getChunk(std::fstream & inputFile, unsigned int & chunkLenght, unsigned int & chunkName, 
                    std::unique_ptr<unsigned char[]> & rawChunkData){
    
    // Since the data is in big endian, when reading an int we need to first read 4 bytes
    // and then translate those bytes into a int.
    
    std::unique_ptr<unsigned char[]> chunkLenghtByteArray = std::make_unique<unsigned char[]>(4);
    std::unique_ptr<unsigned char[]> chunkNameByteArray = std::make_unique<unsigned char[]>(4);
    std::unique_ptr<unsigned char[]> chunkCRCByteArray = std::make_unique<unsigned char[]>(4);

    unsigned int chunkCRC;

    // Chunk lenght
    inputFile.read(reinterpret_cast<char*>(chunkLenghtByteArray.get()), sizeof(unsigned int));
    chunkLenght = extractBigEndianUInt(chunkLenghtByteArray, 0);

    // Chunk Name
    // Since the value is only used in comparisons, it doesn't matter if its in little endian
    inputFile.read(reinterpret_cast<char*>(chunkNameByteArray.get()), sizeof(unsigned int));
    chunkName = extractBigEndianUInt(chunkNameByteArray, 0);

    // Chunk Data
    rawChunkData = std::make_unique<unsigned char[]>(chunkLenght);

    inputFile.read(reinterpret_cast<char*>(rawChunkData.get()), chunkLenght);

    // Chunk CRC
    inputFile.read(reinterpret_cast<char*>(chunkCRCByteArray.get()), sizeof(unsigned int));
    chunkCRC = extractBigEndianUInt(chunkCRCByteArray, 0);

    if(crc(chunkNameByteArray, rawChunkData, chunkLenght) != chunkCRC){
        std::cout << "The PNG is corrupted and can not be salfely translated to another format\n";
        return -1;
    }

    return 0;

}


int getBytesPerPixelPNG(unsigned char colorType){
    switch(colorType){
        case 0: // Grayscale
            return 1;
        case 2: // RGB
            return 3;
        case 5: // Grayscale with alpha values (transparency)
            return 2;
        case 6: // RGB with alpha values (transparency)
            return 4;
    }

    std::cerr << "The color type of the image is not currently supported by the program\n";
    return -1;
}

int obtainGetPixelFunctionPNG(unsigned char colorType, 
                            std::function<Pixel(std::unique_ptr<unsigned char[]> &, int)> & getPixelFunction){
    switch(colorType){
        case 0: // Grayscale
            getPixelFunction = grayToPixel;
            break;
        case 2: // RGB
            getPixelFunction = rgbToPixel;
            break;
        case 5: // Grayscale with alpha values (transparency)
            getPixelFunction = grayWithAlphaToPixel;
            break;
        case 6: // RGB with alpha values (transparency)
            getPixelFunction = rgbWithAlphaToPixel;
            break;
        default:
            std::cerr << "The color type of the image is not currently supported by the program\n";
            return -1;
    }

    return 0;
}