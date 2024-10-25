
#include <vector>
#include <memory>

#include "dataStructures.hpp"

#include "jpgStructs.hpp"
#include "jpgEnums.hpp"

/// Data to Pixels

void blocksToPixels(unsigned short height, unsigned short width, JpgType jpgType,
                    std::vector<Component> & components,  std::unique_ptr<Pixel []> & outputPixels);