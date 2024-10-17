#include "jpgHuffmanTree.hpp"

#include <iostream>




JpgHuffmanTree::JpgHuffmanTree(std::unique_ptr<unsigned char[]> & fileData, unsigned int offSet, 
                               unsigned short segmentLenght, int & err){

    unsigned char charsWithThisLenght;
    int numCharAdded = 0;
    std::vector<std::weak_ptr<HuffmanNode>> availableLeafs;
    std::shared_ptr<HuffmanNode> currentLeaf;
    std::shared_ptr<HuffmanNode> newLeaf;


    id = fileData[offSet];
    rootLeaf = std::make_shared<HuffmanNode>();
    rootLeaf->leafDepth = 0;

    leafList.push_back(rootLeaf);
    availableLeafs.push_back(rootLeaf);

    /// This loop goes through the first 16 bytes after the table ID
    /// where it is stored the number of character in each level of the tree
    for(int depth = 1; depth<17; ++depth){
        
        charsWithThisLenght = fileData[offSet + depth];

        /// Once we have the number of characters in this level we add their leaves to the tree
        for(int j = 0; j<charsWithThisLenght; ++j){
            
            currentLeaf = availableLeafs[availableLeafs.size()-1].lock();

            while(currentLeaf->leafDepth != depth){
                newLeaf = std::make_shared<HuffmanNode>();
                newLeaf->leafDepth = currentLeaf->leafDepth + 1;
                
                leafList.push_back(newLeaf);

                if(currentLeaf->leftLeaf.expired()){

                    currentLeaf->leftLeaf = newLeaf;
                }else{
                    availableLeafs.pop_back();

                    currentLeaf->rightLeaf = newLeaf;
                }

                availableLeafs.push_back(newLeaf);
                currentLeaf = newLeaf;
            }


            currentLeaf->hasCharacter = true;
            currentLeaf->character = fileData[offSet+ 17 + numCharAdded];

            availableLeafs.pop_back();
            ++numCharAdded;
        }
    }

    if((numCharAdded+19!=segmentLenght)){
        err = -1;
        std::cerr << "Could not create one of the Huffman trees because the segement lenght is of "
        << segmentLenght << " while the actual size of the table is " << numCharAdded+19 << ".\n";
        return;
    }
}