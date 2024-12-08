
#include <vector>

#include "jpgStructs.hpp"

// DCT means Discrete Cosine Transform

/// @brief Performs DCT on all the JpgBlocks inside blocks
/// @param blocks A vector with all the blocks of a component
void applyDCT(std::vector<JpgBlock> & blocks);

/// @brief Performs the inverseDCT on all the JpgBlocks inside blocks
/// @param blocks A vector with all the blocks of a component
void inverseDCT(std::vector<JpgBlock> & blocks);