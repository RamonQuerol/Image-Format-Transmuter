#include <vector>

#include "jpgStructs.hpp"
#include "jpgHuffmanTree.hpp"


int decompressJpgBlock(std::unique_ptr<unsigned char []> & scanData, 
                       unsigned int & byteOffset, unsigned int & bitOffset, 
                       JpgHuffmanTree & dcHuffmanTree, JpgHuffmanTree & acHuffmanTree,
                       unsigned char (& zigzagTable)[64], JpgBlock & outputBlock);