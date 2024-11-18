
#include <vector>

#include "jpgHuffmanTree.hpp"

#ifndef JPG_STRUCTS_H
#define JPG_STRUCTS_H


struct JpgBlock{
    int blockData[64] = {0};
};

struct QuantificationTable{
    unsigned char data[64];
};

struct Component{
    unsigned char componentID;
    unsigned char horizontalSampling;
    unsigned char verticalSampling;
    unsigned char quatizationTable;
    unsigned char huffmanTableDC;
    unsigned char huffmanTableAC;
    int prevDC = 0;
    std::vector<JpgBlock> blocks;
};

struct ComponentDataInfo{
    unsigned int componentID;
    unsigned char huffmanTableDC;
    unsigned char huffmanTableAC;
};



struct DataInfo{
    unsigned int scanDataSize;
    std::unique_ptr<unsigned char []> scanData;

    unsigned char numComponents;
    std::vector<ComponentDataInfo> component;

    unsigned char startSpectral;
    unsigned char endSpectral;
    unsigned char currentRefinementPos;
    unsigned char newRefinementPos;

    std::vector<JpgHuffmanTree> acTrees;
};

#endif

