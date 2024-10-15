

struct Component{
    unsigned char componentID;
    unsigned char sampligFactors;
    unsigned char quatizationTable;
    unsigned char huffmanTableDC;
    unsigned char huffmanTableAC;
};


void assignHuffTablesToComponent(unsigned char huffTableByte, Component & component);
