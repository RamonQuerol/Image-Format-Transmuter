#include "jpgCodecFunctions.hpp"

// Standar libraries
#include <memory>
#include <cstring>
#include <iostream>

// Other files
#include "fileDataManagementUtils.hpp"
#include "jpgComponentManagement.hpp"
#include "jpgSegmentManagement.hpp"
#include "jpgPixelManagement.hpp"
#include "jpgDiffEncoding.hpp"
#include "jpgQuantization.hpp"
#include "jpgHuffmanTree.hpp"
#include "jpgCompression.hpp"
#include "jpgStructs.hpp"
#include "jpgZigzag.hpp"
#include "jpgDCT.hpp"

#include "configEnums.hpp"

#define START_SEGEMENT_MARKER 55551 /// FF D8
#define APP0_SEGMENT_MARKER 57599 /// FF E0
#define APP1_SEGMENT_MARKER 57855 // FF E1
#define QUANTIZATION_SEGMENT_MARKER 56319 // FF DB
#define BASELINE_SEGMENT_MARKER 49407 // FF C0
#define PROGRESSIVE_SEGEMENT_MARKER 49919 /// FF C2
#define HUFFMAN_SEGMENT_MARKER 50431 /// FF C4
#define SCAN_SEGMENT_MARKER 56063 /// FF DA
#define COMMENT_SEGMENT_MARKER 65279 // FF FE
#define END_SEGMENT_MARKER 55807 /// FF D9


int decodeJPG(std::fstream & inputFile, Image & decodedImage){

    //// Variables

    /// File reading and analizing variables
    unsigned int fileSize = getFileSize(inputFile);
    std::unique_ptr<unsigned char[]> fileData;

    int remainingBytes;
    unsigned int fileDataOffset = 0;
    unsigned char *fileDataPointer; /// This pointer should never be freed
    

    /// Segment managing
    unsigned short segmentMarker;
    unsigned short segmentLenght;
    bool reachedEndSegment = false;

    std::unique_ptr<unsigned char[]> scanData;
    unsigned int scanDataSize;

    /// Image characteristics
    unsigned char bitsPerPixels;
    unsigned short height;
    unsigned short width;

    /// Component managing
    std::vector<Component> components;
    Component tempComponent;
    int numComponents = 0;

    /// Huffman trees
    std::vector<JpgHuffmanTree> dcHuffmanTrees; 
    std::vector<JpgHuffmanTree> acHuffmanTrees;
    unsigned int huffmanByteOffset = 0;
    unsigned int huffmanBitOffset = 7;

    /// Blocks
    JpgBlock tempBlock;

    unsigned char zigzagTable[64];

    /// Quantization

    std::vector<QuantificationTable> quantizationTables;

    QuantificationTable tempQuantTable;

    /// Result variables
    std::unique_ptr<Pixel []> imagePixels;
    Metadata metadata;

    /// Suport variables
    int err = 0;

    //// Reading the file

    fileData = std::make_unique<unsigned char[]>(fileSize); // This pointer will be reseted once the while is done
    inputFile.read(reinterpret_cast<char *>(fileData.get()), fileSize);

    remainingBytes = fileSize;
    fileDataPointer = fileData.get();

    while(remainingBytes > 1 && !reachedEndSegment){

        memcpy(&segmentMarker, fileDataPointer + fileDataOffset, 2);
        fileDataOffset += 2;
        remainingBytes -= 2;

        switch (segmentMarker){
        case START_SEGEMENT_MARKER:
            break;
        case END_SEGMENT_MARKER:
            reachedEndSegment = true;
            break;
        default: 
            segmentLenght = extractBigEndianUshort(fileData, fileDataOffset);
            
            fileDataOffset += 2;
            remainingBytes = remainingBytes - segmentLenght; 
            switch(segmentMarker){
                case BASELINE_SEGMENT_MARKER:
                    bitsPerPixels = fileData[fileDataOffset];
                    height = extractBigEndianUshort(fileData, fileDataOffset+1);
                    width = extractBigEndianUshort(fileData, fileDataOffset +3);

                    numComponents = fileData[fileDataOffset + 5];

                    for(int i = 0; i<numComponents; ++i){
                        tempComponent.componentID = fileData[fileDataOffset+6+i*3];
                        getComponentSamplingFactors(fileData[fileDataOffset + 7 + i*3], tempComponent);
                        tempComponent.quatizationTable = fileData[fileDataOffset + 8 + i*3];
                        
                        components.push_back(tempComponent);
                    }

                    break;
                case PROGRESSIVE_SEGEMENT_MARKER:
                    std::cerr << "The program does not currently support progressive DCT-based jpeg\n";
                    return -1;
                case QUANTIZATION_SEGMENT_MARKER:
                    if(segmentLenght!=67){

                        if(segmentLenght>67){
                            std::cerr << "One of the quantization tables in the input jpeg has more bytes than it is supossed to have";
                        }else {
                            std::cerr << "One of the quantization tables in the input jpeg has less bytes than it is supossed to have";
                        }
                        return -1;
                    }

                    memcpy(&tempQuantTable, fileDataPointer + fileDataOffset + 1, 64);
                    quantizationTables.push_back(tempQuantTable);
                    break;

                case HUFFMAN_SEGMENT_MARKER:
                    if(fileData[fileDataOffset]/16){ // 1 means that is an AC huffman table
                        acHuffmanTrees.push_back(JpgHuffmanTree(fileData, fileDataOffset, segmentLenght, err));
                    }else{ // 0 means that is a DC huffman table
                        dcHuffmanTrees.push_back(JpgHuffmanTree(fileData, fileDataOffset, segmentLenght, err));
                    }
                    break;
                case SCAN_SEGMENT_MARKER:

                    for(int i = 0; i<numComponents; ++i){
                        assignHuffTablesToComponent(fileData[fileDataOffset+2+i*2],components[i]);
                    }
                    
                    fileDataOffset += segmentLenght-2;

                    // Note: The function will change fileDataOffset amd remainingBytes acording to
                    // the size of the scan data inside the file (which is a bit more that scanDataSize)
                    scanDataSize = extractScanData(fileData, fileDataOffset, remainingBytes, scanData);

                    if(scanDataSize < 0){
                        return -1;
                    }
                    
                    continue;

                default:
                    break;
            }

            if(err){
                return -1;
            }

            fileDataOffset += segmentLenght-2;
        }
    }

    fileData.reset(); // We no longer need to store the file data so we just reset it


    createZigzagTable(zigzagTable);


    while(scanDataSize-1>huffmanByteOffset){

        ///TODO: Right now this only allows jpgs with 4:4:4 sampling

        for(auto &component : components){
            
            for(int i = 0; i<component.verticalSampling*component.horizontalSampling; ++i){

                if(decompressJpgBlock(scanData, huffmanByteOffset, huffmanBitOffset, 
                    dcHuffmanTrees[component.huffmanTableDC], acHuffmanTrees[component.huffmanTableAC], 
                    zigzagTable, tempBlock)){
                    return -1;
                }

                component.blocks.push_back(tempBlock);

                std::memset(&tempBlock, 0, sizeof(JpgBlock));
            }
        }
    }


    for(auto &component : components){
        reverseDifferentialEncoding(component.blocks);
    }

    /// At this point every Block its independent of every other block in the image

    /// Quantization

    for(auto &component : components){
        reverseQuantization(quantizationTables[component.quatizationTable], component.blocks);
    }

    /// DCT

    for(auto &component : components){
        inverseDCT(component.blocks);
    }


    /// Translate the data to pixels

    imagePixels = std::make_unique<Pixel []>(height*width);

    yCbCrToPixels(height, width, components[0].blocks, components[1].blocks, components[2].blocks, imagePixels);

    /// Last metadata

    metadata.colorType = COLOR;

    /// Add everything to decodedImage

    decodedImage.metadata = metadata;
    decodedImage.heigth = height;
    decodedImage.width = width;
    decodedImage.imageData = move(imagePixels);

    return 0;

}