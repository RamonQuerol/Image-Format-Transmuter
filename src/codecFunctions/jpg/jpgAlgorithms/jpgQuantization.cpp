#include "jpgQuantization.hpp"

#include <iostream>

void reverseQuantization(QuantificationTable & table, std::vector<JpgBlock> & blocks){

    for(auto &block : blocks){

        for(int i  = 0; i<64; ++i){
            block.blockData[i] *= table.data[i];
        }
    }
}