
#include <vector>

#include "jpgStructs.hpp"

// DCT means Discrete Cosine Transform

/// @brief Performs the inverseDCT on all the JpgBlocks insde blocks
/// @param blocks A vector with all the blocks of a component
void inverseDCT(std::vector<JpgBlock> & blocks);