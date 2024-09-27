#include "fileDataManagementUtils.hpp"

#include <iostream>

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