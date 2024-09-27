#include <memory>

#ifndef PNG_ALGORITHMS_H
#define PNG_ALGORITHMS_H

/// Filter algorithms



/// Compression algorithm


/// CRC (Cyclic Redundancy Check)

/* Returns the CRC of the bytes chunkName + rawData[0..len-1]. */
unsigned long crc(std::unique_ptr<unsigned char[]> & chunkName,std::unique_ptr<unsigned char[]> & rawData, int len);

#endif