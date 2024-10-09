
/// Standard libraries 
#include <iostream>
#include <vector>
#include <functional>

/// Other files
#include "pngCodecFunctions.hpp"
#include "pngChunkManagement.hpp"
#include "pngColorManagement.hpp"
#include "pngPixelManagement.hpp"
#include "fileDataManagementUtils.hpp"
#include "pngCompression.hpp"
#include "pngFiltering.hpp"
#include "pngCRC.hpp"


#define PNG_SIGNATURE 727905341920923785

int encodePNG(std::fstream & outputFile, Image & image){

    ///// Variables /////

    /// Signature
    unsigned long long signature = PNG_SIGNATURE;

    /// Header chunk variables
    unsigned int width = image.width;
    unsigned int height = image.heigth;
    unsigned char bitDepth = 8;
    unsigned char colorType = getCharColorTypePNG(image.metadata.colorType);
    unsigned char filterMethod = 0;
    unsigned char compressMethod = 0;
    unsigned char interlacing = 0;

    /// Chunk management variables
    unsigned int chunkName;
    unsigned int chunkLenght;
    std::unique_ptr<unsigned char[]> chunkData;

    /// Compression and filtering variables
    std::unique_ptr<unsigned char[]> rawData;

    int filteredDataSize;
    std::unique_ptr<unsigned char[]> filteredData; // Data after the filtering process has finished

    std::unique_ptr<std::vector<unsigned int>> idatLenghts;
    std::unique_ptr<std::vector<std::unique_ptr<unsigned char[]>>> compressedData; // Is a vector that contains all the data from all the IDAT block in the PNG

    /// Support variables

    int numPixels = height*width;
    int bytesPerPixel = getBytesPerPixelPNG(colorType);
    std::unique_ptr<unsigned char[]> iendByteArray;


    ///// Image data management /////

    /// Translation to byte array

    rawData = std::make_unique<unsigned char[]>(numPixels*bytesPerPixel);

    if(translateToByteArray(move(image.imageData), image.metadata.colorType, numPixels, rawData)){
        return -1;
    }

    /// Filtering

    filteredDataSize = numPixels*bytesPerPixel + height;
    filteredData = std::make_unique<unsigned char[]>(filteredDataSize);

    if(filterPNG(move(rawData), height, width, bytesPerPixel, filteredData)){
        return -1;
    }


    /// Compression

    compressedData = std::make_unique<std::vector<std::unique_ptr<unsigned char[]>>>();
    idatLenghts = std::make_unique<std::vector<unsigned int>>();

    if(compressPNG(move(filteredData),filteredDataSize, compressedData, idatLenghts)){
       return -1; 
    }

    if(compressedData->size() != idatLenghts->size()){
        std::cerr << "Error during the compression of the output png, the bytes array and lenght vectors sizes do not match\n";
    }

    ///// Chunk creation and addition to the output file ///// 

    /// Signature

    outputFile.write(reinterpret_cast<char *>(&signature), sizeof(unsigned long long));

    /// Header chunk

    chunkName = IHDR;
    chunkLenght = 13;
    chunkData = std::make_unique<unsigned char[]>(chunkLenght);

    addBigEndianUInt(width, chunkData, 0);
    addBigEndianUInt(height, chunkData, 4);
    chunkData[8] = bitDepth;
    chunkData[9] = colorType;
    chunkData[10] = compressMethod;
    chunkData[11] = filterMethod;
    chunkData[12] = interlacing;

    if(addChunk(outputFile, chunkLenght, chunkName, chunkData)){
        return -1;
    }

    /// IDAT chunks

    chunkName = IDAT;

    for(int i = 0; i<compressedData->size(); ++i){
        addChunk(outputFile, idatLenghts->at(i), chunkName, compressedData->at(i));
    }

    /// IEND chunk

    chunkName = IEND;
    iendByteArray = std::make_unique<unsigned char[]>(0);
    chunkLenght = 0;

    addChunk(outputFile, chunkLenght, chunkName, iendByteArray);

    return 0;
}

int decodePNG(std::fstream &inputFile, Image &decodedImage){

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
    Metadata metadata;

    /// Support variables
    bool iendChunkReached = false;
    int bytesPerPixel; // The number of bytes each pixel takes in the image data


    //// Signature management ////

    inputFile.read(reinterpret_cast<char *>(&signature), sizeof(unsigned long long));

    if (signature != PNG_SIGNATURE){
        std::cerr << "Not a PNG, the signature does not match\n";
        return -1;
    }



    //// We get the header chunk (IHDR) and extract all the useful data stored inside it

    if (getChunk(inputFile, chunkLenght, chunkName, rawChunkData)){
        return -2;
    }

    if (chunkName != IHDR){
        std::cerr << "The input png file does not have a header chunk\n";
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

    if(translateToPixelArray(move(rawPixelData), colorType, height*width, imagePixels)){
        return -1;
    }



    //// Finally we move the data to the Image object

    decodedImage.heigth = height;
    decodedImage.width = width;
    decodedImage.imageData = move(imagePixels);

    metadata.colorType = getEnumColorTypePNG(colorType);

    if(metadata.colorType == UNDEFINED_COLOR){
        return -1;
    }

    decodedImage.metadata = metadata;

    return 0;
}