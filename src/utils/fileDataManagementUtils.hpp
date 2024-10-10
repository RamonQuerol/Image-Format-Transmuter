#include <memory>

#ifndef FILE_DATA_MANAGEMENT_UTILS_H
#define FILE_DATA_MANAGEMENT_UTILS_H

// Translates the intVal into a 4 bytes positioned in Big Endian order, then it stores those bytes
// inside the byteArray.
void addBigEndianUInt(unsigned int intVal, std::unique_ptr<unsigned char[]> & byteArray, int offset);


// Returns the unsigned int located in the offset from a big endian byte array
//
// PRE: The function asumes that it can extract a int size of data from the byte array
unsigned int extractBigEndianUInt(std::unique_ptr<unsigned char[]> & byteArray, int offset);

#endif