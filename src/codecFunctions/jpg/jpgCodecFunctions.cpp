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
#include "jpgEnums.hpp"
#include "jpgDCT.hpp"

#include "configEnums.hpp"





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

    std::vector<std::unique_ptr<DataInfo>> dataInfoBlocks;
    std::unique_ptr<DataInfo> tempDataInfo = std::make_unique<DataInfo>();

    /// Marker consistancy
    JpgEncoding imageEncoding = UNDEFINED_JPG_ENCODING;

    /// Image characteristics
    unsigned char bitsPerPixels;
    unsigned short height;
    unsigned short width;

    /// Component managing
    std::vector<Component> components;
    int numComponents = 0;
    
    JpgType jpgType;

    /// Huffman trees
    std::vector<JpgHuffmanTree> dcHuffmanTrees; 
    std::vector<JpgHuffmanTree> acHuffmanTrees;

    /// Zigzag table

    unsigned char zigzagTable[64];
    createZigzagTable(zigzagTable);

    /// Quantization

    std::vector<QuantificationTable> quantizationTables;

    QuantificationTable tempQuantTable;

    /// Restart Intervals variables
    unsigned short currentRestartInterval = 0;
    bool usesRestartMarkers = false; /// True if the jpeg has restart modulo markers

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
                    if(imageEncoding != UNDEFINED_JPG_ENCODING){
                        std::cerr << "ERROR: The input image has more than one encoding(baseline, progressive, etc)\n";
                        return -1;
                    }

                    extractHeaderData(fileData, fileDataOffset, bitsPerPixels, height, width,
                                         components, numComponents);
                    
                    imageEncoding = BASELINE_ENCODING;
                    break;
                case PROGRESSIVE_SEGEMENT_MARKER:
                    if(imageEncoding != UNDEFINED_JPG_ENCODING){
                        std::cerr << "ERROR: The input image has more than one encoding(baseline, progressive, etc)\n";
                        return -1;
                    }

                    extractHeaderData(fileData, fileDataOffset, bitsPerPixels, height, width,
                                         components, numComponents);
                    
                    imageEncoding = PROGRESSIVE_ENCODING;
                    break;
                case QUANTIZATION_SEGMENT_MARKER:
                    if(segmentLenght!=67){

                        if(segmentLenght>67){
                            std::cerr << "One of the quantization tables in the input jpeg has more bytes than it is supossed to have";
                        }else {
                            std::cerr << "One of the quantization tables in the input jpeg has less bytes than it is supossed to have";
                        }
                        return -1;
                    }

                    for(int i = 0; i<64; ++i){
                        tempQuantTable.data[zigzagTable[i]] = fileData[fileDataOffset+1+i];
                    }

                    quantizationTables.push_back(tempQuantTable);
                    break;

                case HUFFMAN_SEGMENT_MARKER:
                    if(fileData[fileDataOffset]/16){ // 1 means that is an AC huffman table
                        tempDataInfo->acTrees.push_back(JpgHuffmanTree(fileData, fileDataOffset, segmentLenght, err));
                    }else{ // 0 means that is a DC huffman table
                        dcHuffmanTrees.push_back(JpgHuffmanTree(fileData, fileDataOffset, segmentLenght, err));
                    }
                    break;
                case DEFINE_RESTART_MARKER:
                    currentRestartInterval = extractBigEndianUshort(fileData, fileDataOffset);
                    break;
                case SCAN_SEGMENT_MARKER:

                    tempDataInfo->numComponents = fileData[fileDataOffset];
                    ++fileDataOffset;
                    for(int i = 0; i<tempDataInfo->numComponents; ++i, fileDataOffset += 2){
                        tempDataInfo->component.push_back(createComponentInfo(fileData, fileDataOffset));
                    }

                    tempDataInfo->startSpectral = fileData[fileDataOffset];
                    tempDataInfo->endSpectral = fileData[fileDataOffset+1];

                    tempDataInfo->currentRefinementPos = fileData[fileDataOffset+2]/16;
                    tempDataInfo->newRefinementPos = fileData[fileDataOffset+2]%16;

                    fileDataOffset += 3;

                    // Note: The function will change fileDataOffset amd remainingBytes acording to
                    // the size of the scan data inside the file (which is a bit more that scanDataSize)
                    tempDataInfo->scanDataSize = extractScanData(fileData, fileDataOffset, usesRestartMarkers,
                                                                 remainingBytes, tempDataInfo->scanData);

                    if(tempDataInfo->scanDataSize < 0){
                        return -1;
                    }

                    tempDataInfo->restartInterval = currentRestartInterval;

                    dataInfoBlocks.push_back(move(tempDataInfo));
                    tempDataInfo = std::make_unique<DataInfo>();                    

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


    /// DECOMPRESSING

    switch (imageEncoding){
        case BASELINE_ENCODING:
            if(decompressBaslineJpg(*(dataInfoBlocks[0]), zigzagTable, usesRestartMarkers,
                                    components, dcHuffmanTrees)){
                return -1;
            }
            break;
        case PROGRESSIVE_ENCODING:
            if(decompressProgressiveJpg(dataInfoBlocks, usesRestartMarkers, height, width,
                                        zigzagTable, components, dcHuffmanTrees)){
                return -1;
            }
            break;
        default:
            std::cerr << "ERROR: The program could not find the encoding segment of the image\n";
            return -1;
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
    jpgType = determineJpgType(numComponents, components[0]);

    blocksToPixels(height, width, jpgType, components, imagePixels);


    /// Last metadata

    if(jpgType==GRAY_JPG){
        metadata.colorType = GRAY;
    }else{
        metadata.colorType = COLOR;
    }
    

    /// Add everything to decodedImage

    decodedImage.metadata = metadata;
    decodedImage.heigth = height;
    decodedImage.width = width;
    decodedImage.imageData = move(imagePixels);

    return 0;

}