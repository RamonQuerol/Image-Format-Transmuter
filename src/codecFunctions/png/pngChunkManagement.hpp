#include <memory>
#include <iostream>
#include <fstream>

#include "dataStructures.hpp"

/// Chunk names as constants
#define IHDR 1229472850 // Equivalent to 49 48 44 52 in hex or IHDR in ASCII (Big Endian)
#define PLTE 1347179589 // Equivalent to 50 4C 54 45 in hex or PTLE in ASCII (Big Endian)
#define IDAT 1229209940 // Equivalent to 49 44 41 54 in hex or IDAT in ASCII (Big Endian)
#define IEND 1229278788 // Equivalent to 49 45 4E 44 in hex or IEND in ASCII (Big Endian)

/// Note: The hex values are in little endian

// Retrieves a chunk from the inputFile, and distributes the data into 4 variables:
//      - chunkLenght: The lenght of the rawChunkData array
//      - chunkName: The name of the chunk
//      - rawDataChunk: An array of bytes that stores the data in little endian
int getChunk(std::fstream & inputFile, unsigned int & chunkLenght, unsigned int & chunkName, 
                    std::unique_ptr<unsigned char[]> & rawChunkData);


// Returns the number of bytes each pixel of the image takes based on the color type
// or -1 if the color type is not supported
int getBytesPerPixelPNG(unsigned char colorType);
