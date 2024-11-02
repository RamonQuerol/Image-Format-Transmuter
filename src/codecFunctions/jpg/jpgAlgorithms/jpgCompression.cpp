#include "jpgCompression.hpp"

#include <iostream>
#include <cstring>

#include "jpgUtils.hpp"
#include "jpgDiffEncoding.hpp"


/// Block function prototypes

// Uses the huffmanTrees and the zigzagTable to generate the next JpgBlock from the scanData
int decompressBaselineBlock(std::unique_ptr<unsigned char []> & scanData, 
                            unsigned int & byteOffset, unsigned int & bitOffset, 
                            JpgHuffmanTree & dcHuffmanTree, JpgHuffmanTree & acHuffmanTree,
                            unsigned char (& zigzagTable)[64], JpgBlock & outputBlock);


/// Progressive dataInfo function prototypes

int getFirstProgressiveDCs(DataInfo & dataInfo, std::vector<Component> & components,
                           unsigned short blockHeight, unsigned short blockWidth,
                           std::vector<JpgHuffmanTree> dcHuffmanTrees);


/// Refinement function prototypes

int applyRefinementToDC(DataInfo & dataInfo, std::vector<Component> & components,
                        unsigned short blockHeight, unsigned short blockWidth);


/// Support function prototypes


// Gets the coefficient store in the next coeffLenght bits in scanData and moves the offsets
int getCoefficient(std::unique_ptr<unsigned char []> & scanData, 
                   unsigned int & byteOffset, unsigned int & bitOffset, unsigned char coeffLenght);



/// Main functions


int decompressBaslineJpg(DataInfo & dataInfo, unsigned char (& zigzagTable)[64],
                         std::vector<Component> & components, 
                         std::vector<JpgHuffmanTree> & dcHuffmanTrees){

    JpgBlock tempBlock;
    unsigned int huffmanByteOffset = 0;
    unsigned int huffmanBitOffset = 7;

    for(int i = 0; i < components.size(); ++i){
        components[i].huffmanTableDC = dataInfo.component[i].huffmanTableDC;
        components[i].huffmanTableAC = dataInfo.component[i].huffmanTableAC;
    }

    while(dataInfo.scanDataSize-1>huffmanByteOffset){

        for(auto &component : components){
            
            for(int i = 0; i<component.verticalSampling*component.horizontalSampling; ++i){

                if(decompressBaselineBlock(dataInfo.scanData, huffmanByteOffset, huffmanBitOffset, 
                    dcHuffmanTrees[component.huffmanTableDC], dataInfo.acTrees[component.huffmanTableAC], 
                    zigzagTable, tempBlock)){
                    return -1;
                }

                component.blocks.push_back(tempBlock);

                std::memset(&tempBlock, 0, sizeof(JpgBlock));
            }
        }
    }

    /// Once we have all the blocks we reverse differential encoding
    for(auto &component : components){
        reverseDifferentialEncoding(component.blocks);
    }
    
    return 0;

}


int decompressProgressiveJpg(std::vector<std::unique_ptr<DataInfo>> & dataInfoBlocks, 
                             unsigned short height, unsigned short width,
                             unsigned char (& zigzagTable)[64], std::vector<Component> & components, 
                             std::vector<JpgHuffmanTree> & dcHuffmanTrees){

    int err = 0;
    unsigned short blockHeight = height/(8*components[0].verticalSampling) + (height%(8*components[0].verticalSampling) ? 1 : 0);
    unsigned short blockWidth = width/(8*components[0].horizontalSampling) + (width%(8*components[0].horizontalSampling) ? 1 : 0);

    for(auto &component : components){
        component.blocks.resize(blockHeight*blockWidth*component.verticalSampling*component.horizontalSampling);
    }


    for(auto &dataInfo : dataInfoBlocks){

        if(dataInfo->endSpectral == 0){ /// DC Block

            if(dataInfo->currentRefinementPos == 0){
                err = getFirstProgressiveDCs(*dataInfo, components, blockHeight, blockWidth, dcHuffmanTrees);

                
                /// In progressive jpgs the differential encoding it's applied in the first DC block
                /// so we reverse it now
                for(auto &component : components){
                    reverseDifferentialEncoding(component.blocks);
                }

            }else{
                err = applyRefinementToDC(*dataInfo, components, blockHeight, blockWidth);
            }

        }else{ /// AC Block

            if(dataInfo->currentRefinementPos == 0){

            }else{

            }

        }
        
        if(err){
            return -1;
        }

    }

    return 0;
}


/// Block functions

int decompressBaselineBlock(std::unique_ptr<unsigned char []> & scanData, 
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

/// Progressive dataInfo functions

int getFirstProgressiveDCs(DataInfo & dataInfo, std::vector<Component> & components,
                           unsigned short blockHeight, unsigned short blockWidth,
                           std::vector<JpgHuffmanTree> dcHuffmanTrees){

    int err = 0;
    int coeff;
    unsigned char coeffLenght;
    unsigned int byteOffset = 0;
    unsigned int bitOffset = 7;
    unsigned int subSamplingMult = 3;
    int p;

    for(int i = 0; i<dataInfo.numComponents; ++i){
        components[i].huffmanTableDC = dataInfo.component[i].huffmanTableDC;
    }


    for(int i = 0; i<blockHeight*blockWidth; ++i){

        if(dataInfo.scanDataSize<=byteOffset){
            std::cerr << "ERROR: The first data segment has less data than expected\n";
            return -1;
        }

        for(auto &component : components){

            subSamplingMult = component.verticalSampling*component.horizontalSampling;
            for(int j = 0; j<subSamplingMult; ++j){
                
                coeffLenght = dcHuffmanTrees[component.huffmanTableDC].decodeChar(dataInfo.scanData, byteOffset, bitOffset, err);

                if(err){
                    return -1;
                }

                if(coeffLenght>11){
                    std::cerr << "Error: " << coeffLenght << " its not a valid lenght for a DC value\n";
                    return -1;
                }


                if(coeffLenght!=0){ // If its 0, the result should be 0, which is already the value outputBlock.blockData[0] has
                    coeff = getCoefficient(dataInfo.scanData, byteOffset, bitOffset, coeffLenght);
                    component.blocks[i*subSamplingMult + j].blockData[0] =  coeff << dataInfo.newRefinementPos;
                }
            }
        }
    }

    return 0;
}


/// Refinement functions

int applyRefinementToDC(DataInfo & dataInfo, std::vector<Component> & components,
                        unsigned short blockHeight, unsigned short blockWidth){

    unsigned int subSamplingMult;
    unsigned int byteOffset = 0;
    unsigned int bitOffset = 7;
    unsigned char currentByte = dataInfo.scanData[byteOffset];
    unsigned char bitMultiplier = 1 << bitOffset; 

    for(int i = 0; i < blockHeight*blockWidth; ++i){
        
        for(auto &component : components){

            subSamplingMult = component.verticalSampling*component.horizontalSampling;

            for(int j = 0; j<subSamplingMult; ++j){
                if(currentByte & bitMultiplier){
                    component.blocks[i*subSamplingMult + j].blockData[0] += 1 << dataInfo.newRefinementPos;
                }   

                if(moveBitOffset(byteOffset, bitOffset, bitMultiplier)){
                    currentByte = dataInfo.scanData[byteOffset];
                }
            }
        }

    }

    return 0;
}


/// Support functions

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