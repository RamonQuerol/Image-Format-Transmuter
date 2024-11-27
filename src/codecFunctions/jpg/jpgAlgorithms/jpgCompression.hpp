#include <vector>

#include "jpgStructs.hpp"
#include "jpgHuffmanTree.hpp"


int decompressBaslineJpg(DataInfo & dataInfo, unsigned char (& zigzagTable)[64],
                         unsigned short restartInterval, bool usesRestartMarkers,
                         std::vector<Component> & components, 
                         std::vector<JpgHuffmanTree> & dcHuffmanTrees);



int decompressProgressiveJpg(std::vector<std::unique_ptr<DataInfo>> & dataInfoBlocks,
                             unsigned short restartInterval, bool usesRestartMarkers,
                             unsigned short height, unsigned short width,
                             unsigned char (& zigzagTable)[64], std::vector<Component> & components, 
                             std::vector<JpgHuffmanTree> & dcHuffmanTrees);