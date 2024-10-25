
#include <vector>

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
    std::vector<JpgBlock> blocks;
};

#endif

