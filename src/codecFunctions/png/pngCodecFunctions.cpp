#include <iostream>

#include "pngCodecFunctions.hpp"
#include "pngChunkManagement.hpp"
#include "fileDataManagementUtils.hpp"
#include "pngCompression.hpp"
#include "pngFiltering.hpp"
#include <vector>
#include <functional>

#define PNG_SIGNATURE 727905341920923785

int decodePNG(std::fstream &inputFile, Image &decodedImage)
{

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

    /// Compression and filtering variables
    std::unique_ptr<std::vector<std::unique_ptr<unsigned char[]>>> compressedData; // Is a vector that contains all the data from all the IDAT block in the PNG
    std::unique_ptr<std::vector<unsigned int>> idatLenghts;
    std::unique_ptr<unsigned char[]> filteredData; // Data after decompression but before filtering
    int filteredDataSize;
    
    std::unique_ptr<unsigned char[]> rawPixelData;

    /// Result variables
    std::unique_ptr<Pixel[]> imagePixels;

    /// Support variables
    bool iendChunkReached = false;
    int bytesPerPixel; // The number of bytes each pixel takes in the image data
    std::function<Pixel(std::unique_ptr<unsigned char[]> &, int)> getPixelFunction;


    //// Signature management ////

    inputFile.read(reinterpret_cast<char *>(&signature), sizeof(unsigned long long));

    if (signature != PNG_SIGNATURE){
        std::cout << "Not a PNG, the signature does not match\n";
        return -1;
    }

    if (getChunk(inputFile, chunkLenght, chunkName, rawChunkData)){
        return -2;
    }

    if (chunkName != IHDR){
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

    bytesPerPixel = getBytesPerPixelPNG(colorType);

    if(bytesPerPixel==-1){
        return -4;
    }

    compressedData = std::make_unique<std::vector<std::unique_ptr<unsigned char[]>>>();
    idatLenghts = std::make_unique<std::vector<unsigned int>>();

    while (!iendChunkReached){

        if (getChunk(inputFile, chunkLenght, chunkName, rawChunkData)){
            return -2;
        }

        switch (chunkName){
        case IDAT:
            compressedData->push_back(std::move(rawChunkData));
            idatLenghts->push_back(chunkLenght);
            break;
        case IEND:
            iendChunkReached = true;
            break;
        }


    }

    filteredDataSize = height * (width * bytesPerPixel + 1);
    filteredData = std::make_unique<unsigned char[]>(filteredDataSize);
    
    if(decompressPNG(move(compressedData), move(idatLenghts), filteredData, filteredDataSize)){
        return -1;
    }


    // Decompression check (Dups the data to a file)
    // std::fstream outputFile;
    // outputFile.open("outputFilt.dat", std::ios::out | std::ios::binary);

    // if (!outputFile){
    //     std::cout << "The file " << "outputFile" << " could not be opened\n";
    //     return -4;
    // }

    //     outputFile.write(reinterpret_cast<char*>(filteredData.get()), filteredDataSize);


    // outputFile.close();

    rawPixelData = std::make_unique<unsigned char[]>(height * width * bytesPerPixel);


    if(unfilterPNG(move(filteredData), height, width, bytesPerPixel, rawPixelData)){
        return -1;
    }

    // Filtering check (Dups the data to a file)
    // outputFile.open("output.dat", std::ios::out | std::ios::binary);

    // if (!outputFile){
    //     std::cout << "The file " << "outputFile" << " could not be opened\n";
    //     return -4;
    // }

    // outputFile.write(reinterpret_cast<char*>(rawPixelData.get()), height * width * bytesPerPixel);

    // outputFile.close();


    imagePixels = std::make_unique<Pixel[]>(height*width);

    if(obtainGetPixelFunctionPNG(colorType, getPixelFunction)){
        return -1;
    }

    for(int i = 0; i<height*width;++i){
        imagePixels[i] = getPixelFunction(rawPixelData, i*bytesPerPixel);
    }


    decodedImage.heigth = height;
    decodedImage.width = width;
    decodedImage.imageData = move(imagePixels);

    return 0;
}