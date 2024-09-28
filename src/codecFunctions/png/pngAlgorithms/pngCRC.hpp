#include <memory>

#ifndef PNG_CRC_H
#define PNG_CRC_H

/* Returns the CRC of the bytes chunkName + rawData[0..len-1]. */
unsigned long crc(std::unique_ptr<unsigned char[]> & chunkName,std::unique_ptr<unsigned char[]> & rawData, int len);

#endif