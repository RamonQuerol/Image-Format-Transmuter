#include <vector>

#include "jpgStructs.hpp"
#include "jpgHuffmanTree.hpp"

// Uses the huffmanTrees and the zigzagTable to generate the next JpgBlock from the scanData
int decompressJpgBlock(std::unique_ptr<unsigned char []> & scanData, 
                       unsigned int & byteOffset, unsigned int & bitOffset, 
                       JpgHuffmanTree & dcHuffmanTree, JpgHuffmanTree & acHuffmanTree,
                       unsigned char (& zigzagTable)[64], JpgBlock & outputBlock);