#include "jpgCompression.hpp"

#include <iostream>


int decompressJpgBlock(std::unique_ptr<unsigned char []> & scanData, 
                       unsigned int & byteOffset, unsigned int & bitOffset, 
                       std::vector<JpgHuffmanTree> & huffmanTrees, int dcTreePos, int acTreePos,
                       JpgBlock & outputBlock){

    int err = 0;
    unsigned char compressedAC;
    unsigned char numOfZeros;

    outputBlock.blockData[0] = huffmanTrees[dcTreePos].decodeChar(scanData, byteOffset, bitOffset, err);

    for(int i = 1; i<64 && !err; ++i){

        compressedAC = huffmanTrees[acTreePos].decodeChar(scanData, byteOffset, bitOffset, err);

        numOfZeros = compressedAC/16;
        
        for(int j = 0; j<numOfZeros && i<64; ++j, ++i){
            outputBlock.blockData[i] = 0;
        }

        if(i>=64){
            std::cerr << "Error: During decompression the data of one of the blocks exceeded the usual block size of 64\n";
            return -1;
        }

        outputBlock.blockData[i] = compressedAC%16;    
    }

    return err;
}