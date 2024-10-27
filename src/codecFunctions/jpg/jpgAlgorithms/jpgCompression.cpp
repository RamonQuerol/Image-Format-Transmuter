#include "jpgCompression.hpp"

#include <iostream>
#include <cstring>

#include "jpgUtils.hpp"


/// Gets the coefficient store in the next coeffLenght bits in scanData and moves the offsets
int getCoefficient(std::unique_ptr<unsigned char []> & scanData, 
                   unsigned int & byteOffset, unsigned int & bitOffset, unsigned char coeffLenght){
    
    int coeff;
    unsigned char currentByte = scanData[byteOffset];
    unsigned char bitMultiplier = 1 << bitOffset;    
    
    if(currentByte & bitMultiplier){
        coeff = 1 << (coeffLenght-1);
    }else{
        coeff = -1*(1 << coeffLenght) + 1;
    }

    if(moveBitOffset(byteOffset, bitOffset, bitMultiplier)){
        currentByte = scanData[byteOffset];
    }

    for(int i = coeffLenght-2 ; i>=0; --i){
        
        if(currentByte & bitMultiplier){
            coeff += 1 << i;
        }

        if(moveBitOffset(byteOffset, bitOffset, bitMultiplier)){
            currentByte = scanData[byteOffset];
        }
    }    

    return coeff;
}



int decompressJpgBlock(std::unique_ptr<unsigned char []> & scanData, 
                       unsigned int & byteOffset, unsigned int & bitOffset, 
                       JpgHuffmanTree & dcHuffmanTree, JpgHuffmanTree & acHuffmanTree,
                       unsigned char (& zigzagTable)[64], JpgBlock & outputBlock){

    int err = 0;
    unsigned char compressedAC;
    unsigned char numOfZeros;
    unsigned char coeffLenght;

    /// DC coefficient

    coeffLenght = dcHuffmanTree.decodeChar(scanData, byteOffset, bitOffset, err);

    if(coeffLenght>11){
        std::cerr << "Error: " << coeffLenght << " its not a valid lenght for a DC value\n";
        return -1;
    }

    if(coeffLenght!=0){ // If its 0, the result should be 0, which is already the value outputBlock.blockData[0] has
        outputBlock.blockData[0] = getCoefficient(scanData, byteOffset, bitOffset, coeffLenght);
    }


    /// AC coefficients
    for(int i = 1; i<64 && !err; ++i){

        compressedAC = acHuffmanTree.decodeChar(scanData, byteOffset, bitOffset, err);

        numOfZeros = compressedAC/16;
        

        i += numOfZeros;

        if(i>=64){
            std::cerr << "Error: During decompression the data of one of the blocks exceeded the usual block size of 64\n";
            return -1;
        }

        coeffLenght = compressedAC%16;

        if(coeffLenght>10){
            std::cerr << "Error: " << coeffLenght << " its not a valid lenght for a AC value\n";
            return -1;
        }

        // If its 00 the rest of the chunk its filled with 0s
        if(compressedAC == 0){
            break;
        }

        if(coeffLenght != 0){
            outputBlock.blockData[zigzagTable[i]] = getCoefficient(scanData, byteOffset, bitOffset, coeffLenght);
        }
    }

    return err;
}