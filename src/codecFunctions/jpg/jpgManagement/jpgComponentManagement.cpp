#include "jpgComponentManagement.hpp"

void assignHuffTablesToComponent(unsigned char huffTableByte, Component & component){
    component.huffmanTableDC = huffTableByte/16;
    component.huffmanTableAC = huffTableByte%16;
}


void getComponentSamplingFactors(unsigned char samplingByte, Component & component){
    component.horizontalSampling = samplingByte/16;
    component.verticalSampling = samplingByte%16;
}