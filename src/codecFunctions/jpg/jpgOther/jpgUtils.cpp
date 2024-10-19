#include "jpgUtils.hpp"


bool moveBitOffset(unsigned int & byteOffset , unsigned int & bitOffset, unsigned char & bitMultiplier){

    --bitOffset;
    bitMultiplier /= 2;

    if(bitOffset>7){// Its >7 because since its unsigned it overflows to a higher number
        bitOffset = 7;
        ++byteOffset;
        bitMultiplier = 128;
        return true;
    }

    return false;
}