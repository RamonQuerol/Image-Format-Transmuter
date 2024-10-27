#include "jpgDiffEncoding.hpp"

void reverseDifferentialEncoding(std::vector<JpgBlock> & componentBlocks){ 

    for(int i = 1; i<componentBlocks.size(); ++i){
        componentBlocks[i].blockData[0] += componentBlocks[i-1].blockData[0];
    }

}