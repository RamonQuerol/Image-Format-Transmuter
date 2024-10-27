#include "jpgComponentManagement.hpp"

void assignHuffTablesToComponent(unsigned char huffTableByte, Component & component){
    component.huffmanTableDC = huffTableByte/16;
    component.huffmanTableAC = huffTableByte%16;
}


void getComponentSamplingFactors(unsigned char samplingByte, Component & component){
    component.horizontalSampling = samplingByte/16;
    component.verticalSampling = samplingByte%16;
}


JpgType determineJpgType(int numComponents, Component & yComponent){
    
    if(numComponents==1){
        return GRAY_JPG;
    }

    if(yComponent.horizontalSampling>1){

        if(yComponent.verticalSampling>1){
            return COLOR_4_2_0;
        }

        return COLOR_4_2_2_HORIZONTAL;
    }

    if(yComponent.verticalSampling>1){
        return COLOR_4_2_2_VERTICAL;
    }

    return COLOR_4_4_4;
}