
/// Standard libraries 
#include <iostream>
#include <vector>
#include <functional>

/// Other files
#include "pngCodecFunctions.hpp"
#include "pngChunkManagement.hpp"
#include "pngColorManagement.hpp"
#include "fileDataManagementUtils.hpp"
#include "pngCompression.hpp"
#include "pngFiltering.hpp"


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


    //// We get the header chunk (IHDR) and extract all the useful data stored inside it

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

    //// Then we extract and handdle all the other chunks

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

    //// After managing all the chunks
    //// We decompress the data extracted from the IDAT chunks

    filteredDataSize = height * (width * bytesPerPixel + 1);
    filteredData = std::make_unique<unsigned char[]>(filteredDataSize);
    
    if(decompressPNG(move(compressedData), move(idatLenghts), filteredData, filteredDataSize)){
        return -1;
    }


    //// Once the data is decompressed
    //// We reverse all the filters that where applied to optimize the compression

    rawPixelData = std::make_unique<unsigned char[]>(height * width * bytesPerPixel);

    if(unfilterPNG(move(filteredData), height, width, bytesPerPixel, rawPixelData)){
        return -1;
    }


    //// Now that we have the raw data, we just need to translate that data into Pixels

    imagePixels = std::make_unique<Pixel[]>(height*width);

    // We assign the rawData to Pixel translator function to the variable getPixelFunction
    if(obtainGetPixelFunctionPNG(colorType, getPixelFunction)){
        return -1;
    }

    for(int i = 0; i<height*width;++i){
        imagePixels[i] = getPixelFunction(rawPixelData, i*bytesPerPixel);
    }


    //// Finally we move the data to the Image object

    decodedImage.heigth = height;
    decodedImage.width = width;
    decodedImage.imageData = move(imagePixels);

    return 0;
}