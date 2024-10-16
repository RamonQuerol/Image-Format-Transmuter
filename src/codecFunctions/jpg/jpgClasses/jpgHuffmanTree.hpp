#include <memory>
#include <vector>

struct HuffmanLeaf{
    bool hasCharacter = false;
    int leafDepth;
    unsigned char character;
    std::weak_ptr<HuffmanLeaf> leftLeaf;
    std::weak_ptr<HuffmanLeaf> rightLeaf;
};

class JpgHuffmanTree{

    private:
        unsigned char id;
        std::shared_ptr<HuffmanLeaf> rootLeaf;
        std::vector<std::shared_ptr<HuffmanLeaf>> leafList;
    public:
        // Constructor for decoder
        JpgHuffmanTree(std::unique_ptr<unsigned char[]> & fileData, unsigned int offSet, 
                        unsigned short segmentLenght, int & err);
};