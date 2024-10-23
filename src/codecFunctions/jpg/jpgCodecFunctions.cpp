#include "jpgCodecFunctions.hpp"

// Standar libraries
#include <memory>
#include <cstring>
#include <iostream>

// Other files
#include "fileDataManagementUtils.hpp"
#include "jpgComponentManagement.hpp"
#include "jpgSegmentManagement.hpp"
#include "jpgDiffEncoding.hpp"
#include "jpgQuantization.hpp"
#include "jpgHuffmanTree.hpp"
#include "jpgCompression.hpp"
#include "jpgStructs.hpp"
#include "jpgZigzag.hpp"
#include "jpgDCT.hpp"

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
    std::unique_ptr<Component []> components;
    int numComponents = 0;

    /// Huffman trees
    std::vector<JpgHuffmanTree> huffmanTrees; 
    unsigned int numHuffTrees = 0;

    unsigned int huffmanByteOffset = 0;
    unsigned int huffmanBitOffset = 7;

    /// Blocks
    std::vector<JpgBlock> yBlocks;
    std::vector<JpgBlock> cbBlocks;
    std::vector<JpgBlock> crBlocks;

    JpgBlock tempBlock;

    unsigned char zigzagTable[64];

    /// Quantization

    std::vector<QuantificationTable> quantizationTables;

    QuantificationTable tempQuantTable;

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
                    components = std::make_unique<Component[]>(numComponents);

                    for(int i = 0; i<numComponents; ++i){
                        components[i].componentID = fileData[fileDataOffset+6+i];
                        components[i].sampligFactors = fileData[fileDataOffset + 7 + i];
                        components[i].quatizationTable = fileData[fileDataOffset + 8 + i];
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
                    huffmanTrees.push_back(JpgHuffmanTree(fileData, fileDataOffset, segmentLenght, err));
                    ++numHuffTrees;
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

        /// Y component
        if(decompressJpgBlock(scanData, huffmanByteOffset, huffmanBitOffset, huffmanTrees, 0, 1, zigzagTable, tempBlock)){
            return -1;
        }


        yBlocks.push_back(tempBlock);


        std::memset(&tempBlock, 0, sizeof(JpgBlock));

        /// Cb component
        if(decompressJpgBlock(scanData, huffmanByteOffset, huffmanBitOffset, huffmanTrees, 2, 3, zigzagTable, tempBlock)){
            return -1;
        }

        cbBlocks.push_back(tempBlock);

        std::memset(&tempBlock, 0, sizeof(JpgBlock));


        /// Cr component
        if(decompressJpgBlock(scanData, huffmanByteOffset, huffmanBitOffset, huffmanTrees, 2, 3, zigzagTable, tempBlock)){
            return -1;
        }

        crBlocks.push_back(tempBlock);

        std::memset(&tempBlock, 0, sizeof(JpgBlock));
    }

    /// Reverse the differential encoding on the DC values

    reverseDifferentialEncoding(yBlocks);
    reverseDifferentialEncoding(cbBlocks);
    reverseDifferentialEncoding(crBlocks);

    /// At this point every Block its independent of every other block in the image

    /// Quantization

    reverseQuantization(quantizationTables[0], yBlocks);
    reverseQuantization(quantizationTables[1], cbBlocks);
    reverseQuantization(quantizationTables[1], crBlocks);

    /// DCT

    inverseDCT(yBlocks);
    inverseDCT(cbBlocks);
    inverseDCT(crBlocks);

    return 0;

}