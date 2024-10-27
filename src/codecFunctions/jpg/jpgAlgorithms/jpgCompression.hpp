#include <vector>

#include "jpgStructs.hpp"
#include "jpgHuffmanTree.hpp"


int decompressBaslineJpg(std::unique_ptr<unsigned char []> & scanData, unsigned int scanDataSize,
                         std::vector<Component> & components, unsigned char (& zigzagTable)[64],
                         std::vector<JpgHuffmanTree> & dcHuffmanTrees, 
                         std::vector<JpgHuffmanTree> & acHuffmanTrees);