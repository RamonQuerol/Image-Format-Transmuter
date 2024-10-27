
#include <vector>

#include "jpgStructs.hpp"


/// Uses table to reververse the quantization process in all the JpgBlock in blocks
void reverseQuantization(QuantificationTable & table, std::vector<JpgBlock> & blocks);