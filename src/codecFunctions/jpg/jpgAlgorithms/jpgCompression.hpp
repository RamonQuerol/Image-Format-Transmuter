#include <vector>

#include "jpgStructs.hpp"
#include "jpgHuffmanTree.hpp"


int decompressJpgBlock(std::unique_ptr<unsigned char []> & scanData, 
                       unsigned int & byteOffset, unsigned int & bitOffset, 
                       std::vector<JpgHuffmanTree> & huffmanTrees, int dcTreePos, int acTreePos,
                       JpgBlock & outputBlock);