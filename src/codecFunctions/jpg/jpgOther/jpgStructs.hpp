

#ifndef JPG_STRUCTS_H
#define JPG_STRUCTS_H


struct Component{
    unsigned char componentID;
    unsigned char sampligFactors;
    unsigned char quatizationTable;
    unsigned char huffmanTableDC;
    unsigned char huffmanTableAC;
};

struct JpgBlock{
    int blockData[64] = {0};
};

struct QuantificationTable{
    unsigned char data[64];
};

#endif

