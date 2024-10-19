#include <memory>
#include <vector>

#ifndef HUFFMAN_TREE_H
#define HUFFMAN_TREE_H

struct HuffmanNode{
    bool hasCharacter = false;
    int leafDepth;
    unsigned char character;
    std::weak_ptr<HuffmanNode> leftNode;
    std::weak_ptr<HuffmanNode> rightNode;
};

class JpgHuffmanTree{

    private:
        unsigned char id;
        std::shared_ptr<HuffmanNode> rootNode;
        std::vector<std::shared_ptr<HuffmanNode>> nodeList;
    public:
        // Constructor for decoder
        JpgHuffmanTree(std::unique_ptr<unsigned char[]> & fileData, unsigned int offSet, 
                        unsigned short segmentLenght, int & err);


        /// @brief Uses the huffman tree to obtain the next character inside scanData, and changes the offset to point to the bit right after the read character
        /// @param scanData Byte array with the image data encoded with huffman coding
        /// @param byteOffset Offset to the byte that contains the next character
        /// @param bitOffset Offset to bit inside the byte where the next character starts
        /// @param err Integer that if its <0 means that there has been an error
        /// @return The decoded version of the next character
        unsigned char decodeChar(std::unique_ptr<unsigned char []> & scanData, 
                                 unsigned int & byteOffset, unsigned int & bitOffset, int & err);

};

#endif