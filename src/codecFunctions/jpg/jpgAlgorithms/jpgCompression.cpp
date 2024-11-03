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


/// Progressive first dataInfo function prototypes

int getFirstProgressiveDCs(DataInfo & dataInfo, std::vector<Component> & components,
                           unsigned short blockHeight, unsigned short blockWidth,
                           std::vector<JpgHuffmanTree> & dcHuffmanTrees);


int getFirstProgressiveACs(DataInfo & dataInfo, Component & component,
                           JpgHuffmanTree & acHuffmanTree, unsigned char (& zigzagTable)[64]);

/// Refinement function prototypes

int applyRefinementToDC(DataInfo & dataInfo, std::vector<Component> & components,
                        unsigned short blockHeight, unsigned short blockWidth);

int applyRefinementToACs(DataInfo & dataInfo, Component & component,
                         JpgHuffmanTree & acHuffmanTree, unsigned char (& zigzagTable)[64]);


/// Support function prototypes


// Gets the coefficient store in the next coeffLength bits in scanData and moves the offsets
int getCoefficient(std::unique_ptr<unsigned char []> & scanData, 
                   unsigned int & byteOffset, unsigned int & bitOffset, unsigned char coeffLength);


int getExtraSkips(std::unique_ptr<unsigned char []> & scanData, 
                       unsigned int & byteOffset, unsigned int & bitOffset, unsigned char numOfZeros);


void skipRefinementZeros(DataInfo & dataInfo, int & blockPos, unsigned char numOfZeros,
                         unsigned int & byteOffset, unsigned int & bitOffset, int refinementPositive,
                         unsigned char (& zigzagTable)[64], JpgBlock & block);


void updateBlockUntilEnd(DataInfo & dataInfo, unsigned char start, int refinementPositive,
                         unsigned int & byteOffset, unsigned int & bitOffset, 
                         unsigned char (& zigzagTable)[64], JpgBlock & block);

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

        if(dataInfo->endSpectral == 0){ /// DC DataInfo

            if(dataInfo->currentRefinementPos == 0){ // First DC DataInfo
                err = getFirstProgressiveDCs(*dataInfo, components, blockHeight, blockWidth, dcHuffmanTrees);

                
                /// In progressive jpgs the differential encoding it's applied in the first DC block
                /// so we reverse it now
                for(auto &component : components){
                    reverseDifferentialEncoding(component.blocks);
                }

            }else{ // Refinement DC DataInfo
                err = applyRefinementToDC(*dataInfo, components, blockHeight, blockWidth);
            }

        }else{ /// AC DataInfo

            if(dataInfo->currentRefinementPos == 0){
                
                for(auto &component : components){
                    component.blocks.resize(blockHeight*blockWidth*component.verticalSampling*component.horizontalSampling);
                }

                err = getFirstProgressiveACs(*dataInfo, components[dataInfo->component[0].componentID],
                    dataInfo->acTrees[0], zigzagTable);

            }else{
                err = applyRefinementToACs(*dataInfo, components[dataInfo->component[0].componentID], dataInfo->acTrees[0], zigzagTable);
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
    unsigned char coeffLength;

    /// DC coefficient

    coeffLength = dcHuffmanTree.decodeChar(scanData, byteOffset, bitOffset, err);

    if(coeffLength>11){
        std::cerr << "Error: " << coeffLength << " its not a valid length for a DC value\n";
        return -1;
    }

    if(coeffLength!=0){ // If its 0, the result should be 0, which is already the value outputBlock.blockData[0] has
        outputBlock.blockData[0] = getCoefficient(scanData, byteOffset, bitOffset, coeffLength);
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

        coeffLength = compressedAC%16;

        if(coeffLength>10){
            std::cerr << "Error: " << coeffLength << " its not a valid length for a AC value\n";
            return -1;
        }

        // If its 00 the rest of the chunk its filled with 0s
        if(compressedAC == 0){
            break;
        }

        if(coeffLength != 0){
            outputBlock.blockData[zigzagTable[i]] = getCoefficient(scanData, byteOffset, bitOffset, coeffLength);
        }
    }

    return err;
}

/// Progressive first dataInfo  functions

int getFirstProgressiveDCs(DataInfo & dataInfo, std::vector<Component> & components,
                           unsigned short blockHeight, unsigned short blockWidth,
                           std::vector<JpgHuffmanTree> & dcHuffmanTrees){

    int err = 0;
    int coeff;
    unsigned char coeffLength;
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
                
                coeffLength = dcHuffmanTrees[component.huffmanTableDC].decodeChar(dataInfo.scanData, byteOffset, bitOffset, err);

                if(err){
                    return -1;
                }

                if(coeffLength>11){
                    std::cerr << "Error: " << coeffLength << " its not a valid length for a DC value\n";
                    return -1;
                }


                if(coeffLength!=0){ // If its 0, the result should be 0, which is already the value outputBlock.blockData[0] has
                    coeff = getCoefficient(dataInfo.scanData, byteOffset, bitOffset, coeffLength);
                    component.blocks[i*subSamplingMult + j].blockData[0] =  coeff << dataInfo.newRefinementPos;
                }
            }
        }
    }

    return 0;
}


int getFirstProgressiveACs(DataInfo & dataInfo, Component & component,
                           JpgHuffmanTree & acHuffmanTree, unsigned char (& zigzagTable)[64]){

    int err = 0;
    int coeff;
    unsigned char compressedAC;
    unsigned char numOfZeros;
    unsigned char coeffLength;
    unsigned int byteOffset = 0;
    unsigned int bitOffset = 7;
    

    for(int i = 0; i<component.blocks.size() && !err; ++i){


        for(int j = dataInfo.startSpectral; j<=dataInfo.endSpectral && !err; ++j){


            compressedAC = acHuffmanTree.decodeChar(dataInfo.scanData, byteOffset, bitOffset, err);

            coeffLength = compressedAC%16;
            numOfZeros = compressedAC/16;
            

            if(coeffLength == 0 && numOfZeros != 15){
                i += (1 << numOfZeros)-1;
                i += getExtraSkips(dataInfo.scanData, byteOffset, bitOffset, numOfZeros);       
                break;
            }
            
            j += numOfZeros;

            

            if(j>dataInfo.endSpectral){
                std::cerr << "Error: During decompression the data of one of the blocks exceeded the endSpectral of " << dataInfo.endSpectral-0 
                        << " in the data block covering the componet with id " << component.componentID-0 << " \n";
                return -1;
            }

            if(coeffLength>10){
                std::cerr << "Error: " << coeffLength << " its not a valid length for a AC value\n";
                return -1;
            }

            if(coeffLength != 0){
                coeff = getCoefficient(dataInfo.scanData, byteOffset, bitOffset, coeffLength);
                component.blocks[i].blockData[zigzagTable[j]] = coeff << dataInfo.newRefinementPos;
            }
        }

    }
    

    return err;
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


int applyRefinementToACs(DataInfo & dataInfo, Component & component,
                         JpgHuffmanTree & acHuffmanTree, unsigned char (& zigzagTable)[64]){

    int err = 0;
    int coeff;
    int skips = 0;
    unsigned char compressedAC;
    unsigned char numOfZeros;
    unsigned char coeffLength;
    unsigned int byteOffset = 0;
    unsigned int bitOffset = 7;
    int refinementPositive = 1 << dataInfo.newRefinementPos;

    

    for(int i = 0; i<component.blocks.size() && !err; ++i){

        for(int j = dataInfo.startSpectral; j<=dataInfo.endSpectral && !err; ++j){

            
            compressedAC = acHuffmanTree.decodeChar(dataInfo.scanData, byteOffset, bitOffset, err);

            coeffLength = compressedAC%16;
            numOfZeros = compressedAC/16;
            

            if(coeffLength > 1){
                std::cerr << "ERROR: During the refinement of AC values the program found a coefficient length different than 0 or 1\n";
                return -1;
            }

            if(coeffLength == 0 && numOfZeros != 15){
                skips += (1 << numOfZeros)-1;
                skips += getExtraSkips(dataInfo.scanData, byteOffset, bitOffset, numOfZeros);             

                updateBlockUntilEnd(dataInfo, j, refinementPositive, 
                    byteOffset, bitOffset, zigzagTable, component.blocks[i]);
                break;
            }

            if(coeffLength==0){// For the case of F0
                skipRefinementZeros(dataInfo, j, 15, byteOffset, bitOffset,
                        refinementPositive, zigzagTable, component.blocks[i]);
                continue;
            }
            

            
            if(j>dataInfo.endSpectral){
                std::cerr << "Error: During decompression the data of one of the blocks exceeded the endSpectral of " << dataInfo.endSpectral-0 
                        << " in the data block covering the componet with id " << component.componentID-0 << " \n";
                return -1;
            }


            coeff = getCoefficient(dataInfo.scanData, byteOffset, bitOffset, coeffLength);


            skipRefinementZeros(dataInfo, j, numOfZeros, byteOffset, bitOffset,
                        refinementPositive, zigzagTable, component.blocks[i]);

            component.blocks[i].blockData[zigzagTable[j]] = coeff << dataInfo.newRefinementPos;
        }

        

        for(; skips > 0; --skips, ++i){
            updateBlockUntilEnd(dataInfo, dataInfo.startSpectral, refinementPositive,
                byteOffset, bitOffset, zigzagTable, component.blocks[i+1]);
        }

    }
    

    return err;

}


/// Support functions

int getCoefficient(std::unique_ptr<unsigned char []> & scanData, 
                       unsigned int & byteOffset, unsigned int & bitOffset, unsigned char coeffLength){
    
    int coeff;
    unsigned char currentByte = scanData[byteOffset];
    unsigned char bitMultiplier = 1 << bitOffset;    
    
    if(currentByte & bitMultiplier){
        coeff = 1 << (coeffLength-1);
    }else{
        coeff = -1*(1 << coeffLength) + 1;
    }

    if(moveBitOffset(byteOffset, bitOffset, bitMultiplier) && coeffLength>1){
        currentByte = scanData[byteOffset];
    }

    for(int i = coeffLength-2 ; i>=0; --i){
        
        if(currentByte & bitMultiplier){
            coeff += 1 << i;
        }

        if(moveBitOffset(byteOffset, bitOffset, bitMultiplier) && i>0){
            currentByte = scanData[byteOffset];
        }
    }    

    return coeff;
}


int getExtraSkips(std::unique_ptr<unsigned char []> & scanData, 
                       unsigned int & byteOffset, unsigned int & bitOffset, unsigned char numOfZeros){

    int coeff = 0;
    unsigned char currentByte = scanData[byteOffset];
    unsigned char bitMultiplier = 1 << bitOffset;  


    for(int i = numOfZeros-1 ; i>=0; --i){
        
        if(currentByte & bitMultiplier){
            coeff += 1 << i;
        }

        if(moveBitOffset(byteOffset, bitOffset, bitMultiplier) && i>0){
            currentByte = scanData[byteOffset];
        }
    }    
    

    return coeff;
}


void skipRefinementZeros(DataInfo & dataInfo, int & blockPos, unsigned char numOfZeros,
                         unsigned int & byteOffset, unsigned int & bitOffset, int refinementPositive,
                         unsigned char (& zigzagTable)[64], JpgBlock & block){

    int bitsRead = 0;
    int remainingZeros = numOfZeros;
    unsigned char currentByte = dataInfo.scanData[byteOffset];
    unsigned char bitMultiplier = 1 << bitOffset;


    while(blockPos < dataInfo.endSpectral){


        if(block.blockData[zigzagTable[blockPos]]==0){
            if(remainingZeros <= 0){
                break;
            }
            ++blockPos;
            --remainingZeros;
            continue;
        }

        if(currentByte & bitMultiplier){// We read a 1

            if(block.blockData[zigzagTable[blockPos]]>0){
                block.blockData[zigzagTable[blockPos]] += refinementPositive;
            }else{
                block.blockData[zigzagTable[blockPos]] -= refinementPositive;
            }         

        }

        ++blockPos;

        if(moveBitOffset(byteOffset, bitOffset, bitMultiplier) && blockPos<dataInfo.endSpectral){
            currentByte = dataInfo.scanData[byteOffset];
        }

    }

}


void updateBlockUntilEnd(DataInfo & dataInfo, unsigned char start, int refinementPositive,
                         unsigned int & byteOffset, unsigned int & bitOffset, 
                         unsigned char (& zigzagTable)[64], JpgBlock & block){

    unsigned char currentByte = dataInfo.scanData[byteOffset];
    unsigned char bitMultiplier = 1 << bitOffset;  


    for(int i = start; i<=dataInfo.endSpectral; ++i){


        /// We only read bits if if the cell has a value already
        if(block.blockData[zigzagTable[i]]!=0){


            if(currentByte & bitMultiplier){// We read a 1


                if(block.blockData[zigzagTable[i]]>0){
                    block.blockData[zigzagTable[i]] += refinementPositive;
                }else{
                    block.blockData[zigzagTable[i]] -= refinementPositive;
                }         

            }

            if(moveBitOffset(byteOffset, bitOffset, bitMultiplier) && i+1<dataInfo.endSpectral){
                currentByte = dataInfo.scanData[byteOffset];
            }

        }
    }
}