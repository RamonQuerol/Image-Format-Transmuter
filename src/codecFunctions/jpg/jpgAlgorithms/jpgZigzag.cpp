#include "jpgZigzag.hpp"

void createZigzagTable(unsigned char (& zigzagTable)[64]){

    int counter = 0;
    int startOfCounter = 0;
    bool polarity = true;

    for(int i = 0; i<36; ++i, --counter){

        if(polarity){
            zigzagTable[i] = counter*8 + startOfCounter-counter;
        }else{
            zigzagTable[i] = (startOfCounter-counter)*8 +counter;
        }

        if(counter == 0){
            ++startOfCounter;
            counter = startOfCounter+1;
            polarity = !polarity; /// Reverse the polarity
        }
    }


    for(int i = 63, j = 0; i>35; --i, ++j){
        zigzagTable[i] = 63-zigzagTable[j];
    }

}