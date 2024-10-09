
#include "pngCompression.hpp"

#include <iostream>
#include <zlib.h>

#include <string.h>
#include <stdexcept>
#include <iomanip>
#include <sstream>


#define MAX_IDAT_CHUNK_SIZE 32768 


/// TODO Test the method (Not possible until i have encodePNG)
int compressPNG(std::unique_ptr<unsigned char[]> decompressedData, unsigned int decomDataSize,
                std::unique_ptr<std::vector<std::unique_ptr<unsigned char[]>>> & compressedData,
                std::unique_ptr<std::vector<unsigned int>> & compressedDataLenghts){
    
    
    int compressionLevel = 5; 
    int ret = Z_OK; /// Result code obtained from zlib's inflate function
    std::unique_ptr<unsigned char[]> outBuffer;
    
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (deflateInit(&zs, compressionLevel) != Z_OK){
        std::cerr << "Error while decompressing the PNG, zlib inflateInit failed\n";
        return -1;
    }

    /// We set the output buffer, since its always the same we do it outside the for
    zs.next_in = reinterpret_cast<Bytef *>(decompressedData.get());
    zs.avail_in = decomDataSize;
    
    while(ret == Z_OK){

        outBuffer = std::make_unique<unsigned char[]>(MAX_IDAT_CHUNK_SIZE);
        zs.next_out = reinterpret_cast<Bytef *>(outBuffer.get());
        zs.avail_out = MAX_IDAT_CHUNK_SIZE;

        ret = deflate(&zs, Z_FINISH);

        compressedData->push_back(move(outBuffer));


        if(ret==Z_OK){
            compressedDataLenghts->push_back(MAX_IDAT_CHUNK_SIZE);
        }
    }

    if (ret != Z_STREAM_END){
        deflateEnd(&zs);
        std::cerr << "Error while compressing the PNG, zlib could not compress all the data from the file\n";
        return -1;
    }
    
    if(zs.total_out%MAX_IDAT_CHUNK_SIZE){
        compressedDataLenghts->push_back(zs.total_out%MAX_IDAT_CHUNK_SIZE);
    }else{
        compressedDataLenghts->push_back(MAX_IDAT_CHUNK_SIZE);
    }

    deflateEnd(&zs);
    
    return 0;
}


int decompressPNG(std::unique_ptr<std::vector<std::unique_ptr<unsigned char[]>>> compressedData, 
                  std::unique_ptr<std::vector<unsigned int>> lenghts, std::unique_ptr<unsigned char[]> &decompressedData, 
                  unsigned int decomDataSize){

    int ret = Z_OK; /// Result code obtained from zlib's inflate function
    
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK){
        std::cerr << "Error while decompressing the PNG, zlib inflateInit failed\n";
        return -1;
    }

    /// We set the output buffer, since its always the same we do it outside the for
    zs.next_out = reinterpret_cast<Bytef *>(decompressedData.get());
    zs.avail_out = decomDataSize;

    /// This will loop through all the IDAT chunks stored in compressedData, decompressiing
    /// them and storing the result in deocompressedData
    for (int i = 0; i < compressedData->size(); ++i){

        zs.next_in = reinterpret_cast<Bytef *>(compressedData->at(i).get());
        zs.avail_in = lenghts->at(i);

        ret = inflate(&zs, 0);


        if (ret != Z_STREAM_END && ret != Z_OK){
            inflateEnd(&zs);
            std::cerr << "Error while decompressing the PNG, zlib could not decompress all the data from the file\n";
            return -1;
        }

    }

    inflateEnd(&zs);

    return 0;
}