#include "jpgComponentManagement.hpp"

void assignHuffTablesToComponent(unsigned char huffTableByte, Component & component){
    component.huffmanTableDC = huffTableByte/16;
    component.huffmanTableAC = huffTableByte%16;
}