
#include "pngCompression.hpp"

#include <iostream>
#include <zlib.h>

#include <string.h>
#include <stdexcept>
#include <iomanip>
#include <sstream>


int compressPNG(std::unique_ptr<unsigned char[]> decompressedData, unsigned int decomDataSize,
                std::unique_ptr<std::vector<std::unique_ptr<unsigned char[]>>> compressedData){
    
    
    return 0;
}


int decompressPNG(std::unique_ptr<std::vector<std::unique_ptr<unsigned char[]>>> compressedData, 
                  std::unique_ptr<std::vector<unsigned int>> lenghts, std::unique_ptr<unsigned char[]> &decompressedData, 
                  unsigned int decomDataSize){

    int ret = Z_OK;
    unsigned int dynamicOffset;
    unsigned int previousLenghts = 0;
    
    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit(&zs) != Z_OK){
        std::cerr << "Error while decompressing the PNG, zlib inflateInit failed\n";
        return -1;
    }

    zs.avail_out = decomDataSize;

    zs.next_out = reinterpret_cast<Bytef *>(decompressedData.get());

    for (int i = 0; i < compressedData->size(); ++i){

        zs.next_in = reinterpret_cast<Bytef *>(compressedData->at(i).get());
        zs.avail_in = lenghts->at(i);

        ret = inflate(&zs, 0);


        if (ret != Z_STREAM_END && ret != Z_OK){
            inflateEnd(&zs);
            std::cerr << "Error while decompressing the PNG, zlib could not decompress all the data from the file\n";
            return -1;
        }

        previousLenghts += lenghts->at(i);
    }

    inflateEnd(&zs);

    return 0;
}