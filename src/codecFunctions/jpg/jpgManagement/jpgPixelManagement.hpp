
#include <vector>
#include <memory>

#include "dataStructures.hpp"

#include "jpgStructs.hpp"

/// Data to Pixels

void yCbCrToPixels(unsigned short height, unsigned short width,
                   std::vector<JpgBlock> & yBlocks, std::vector<JpgBlock> & cbBlocks, std::vector<JpgBlock> & crBlocks,
                   std::unique_ptr<Pixel []> & outputPixels);