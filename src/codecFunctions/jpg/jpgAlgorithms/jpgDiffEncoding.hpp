
#include <vector>

#include "jpgStructs.hpp"

// Reverses the differential encoded on the DC values of the blocks in the componentBlocks vector
void reverseDifferentialEncoding(std::vector<JpgBlock> & componentBlocks);