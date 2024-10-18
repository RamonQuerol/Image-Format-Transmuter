

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
    unsigned char blockData[64];
};


#endif

