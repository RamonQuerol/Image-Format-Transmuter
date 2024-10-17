#include <memory>
#include <vector>

struct HuffmanNode{
    bool hasCharacter = false;
    int leafDepth;
    unsigned char character;
    std::weak_ptr<HuffmanNode> leftLeaf;
    std::weak_ptr<HuffmanNode> rightLeaf;
};

class JpgHuffmanTree{

    private:
        unsigned char id;
        std::shared_ptr<HuffmanNode> rootLeaf;
        std::vector<std::shared_ptr<HuffmanNode>> leafList;
    public:
        // Constructor for decoder
        JpgHuffmanTree(std::unique_ptr<unsigned char[]> & fileData, unsigned int offSet, 
                        unsigned short segmentLenght, int & err);
};