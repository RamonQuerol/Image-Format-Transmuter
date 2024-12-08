
#include <vector>

#include "jpgStructs.hpp"


const QuantificationTable Y90_table(
    {
        3,   2,  2,  3,  5,  8, 10, 12,
        2,   2,  3,  4,  5, 12, 12, 11,
        3,   3,  3,  5,  8, 11, 14, 11,
        3,   3,  4,  6, 10, 17, 16, 12,
        4,   4,  7, 11, 14, 22, 21, 15,
        5,   7, 11, 13, 16, 21, 23, 18,
        10, 13, 16, 17, 21, 24, 24, 20,
        14, 18, 19, 20, 22, 20, 21, 20
    }
);

const QuantificationTable C90_table(
    {
        3,   4,  5,  9, 20, 20, 20, 20,
        4,   4,  5, 13, 20, 20, 20, 20,
        5,   5, 11, 20, 20, 20, 20, 20,
        9,  13, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20
    }
);

/// Uses table to apply the quantization process in all the JpgBlock in blocks
void applyQuantization(QuantificationTable & table, std::vector<JpgBlock> & blocks);


/// Uses table to reverse the quantization process in all the JpgBlock in blocks
void reverseQuantization(QuantificationTable & table, std::vector<JpgBlock> & blocks);