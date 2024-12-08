
#include <vector>
#include <memory>

#include "dataStructures.hpp"

#include "jpgStructs.hpp"
#include "jpgEnums.hpp"

/// Encoding

int pixelsToBlocks(unsigned short height, unsigned short width, JpgType jpgType,
                    std::unique_ptr<Pixel []> & inputPixels, std::vector<Component> & components);

/// Decoding

void blocksToPixels(unsigned short height, unsigned short width, JpgType jpgType,
                    std::vector<Component> & components,  std::unique_ptr<Pixel []> & outputPixels);