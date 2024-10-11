#include "fileDataManagementUtils.hpp"

/// Standard libraries 
#include <iostream>

void addBigEndianUInt(unsigned int intVal, std::unique_ptr<unsigned char[]> & byteArray, int offset){

    unsigned int loopVal = intVal;

    for(int i = 3; i>=0; --i){
        byteArray[offset + i] = loopVal%256;
        loopVal /= 256;
    }
}


// Returns the unsigned int located in the offset from a little endian byte array
//
// PRE: The function asumes that it can extract a int size of data from the byte array
unsigned int extractBigEndianUInt(std::unique_ptr<unsigned char[]> & byteArray, int offset){
    unsigned int result = 0;

    // Since in little endian the lowest value is in the last byte
    for(int i = 3, mult = 1; i>=0; --i, mult *= 256){
        result += byteArray[offset+i]*mult;
    }

    return result;
}