
/// Standard libraries 
#include <memory>
#include <fstream>

/// Other files
#include "dataStructures.hpp"

/// Chunk names as constants
#define IHDR 1229472850 // Equivalent to 49 48 44 52 in hex or IHDR in ASCII (Big Endian)
#define PLTE 1347179589 // Equivalent to 50 4C 54 45 in hex or PTLE in ASCII (Big Endian)
#define IDAT 1229209940 // Equivalent to 49 44 41 54 in hex or IDAT in ASCII (Big Endian)
#define IEND 1229278788 // Equivalent to 49 45 4E 44 in hex or IEND in ASCII (Big Endian)


// Using the data from the variables, inserts a ew chunk into the outputFile:
//      - outputFile: The in which the data will be stored
//      - chunkLenght: The lenght of the chunkData array
//      - chunkName: The name of the chunk
//      - rawDataChunk: An array of bytes that stores the data in little endian
int addChunk(std::fstream & outputFile, unsigned int chunkLenght, unsigned int chunkName, 
                    std::unique_ptr<unsigned char[]> & chunkData);


// Retrieves a chunk from the inputFile, and distributes the data into 3 variables:
//      - chunkLenght: The lenght of the rawChunkData array
//      - chunkName: The name of the chunk
//      - rawDataChunk: An array of bytes that stores the data in little endian
int getChunk(std::fstream & inputFile, unsigned int & chunkLenght, unsigned int & chunkName, 
                    std::unique_ptr<unsigned char[]> & rawChunkData);

