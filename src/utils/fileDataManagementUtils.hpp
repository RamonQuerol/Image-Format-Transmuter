#include <memory>

#ifndef FILE_DATA_MANAGEMENT_UTILS_H
#define FILE_DATA_MANAGEMENT_UTILS_H


// Returns the unsigned int located in the offset from a big endian byte array
//
// PRE: The function asumes that it can extract a int size of data from the byte array
unsigned int extractBigEndianUInt(std::unique_ptr<unsigned char[]> & byteArray, int offset);

#endif