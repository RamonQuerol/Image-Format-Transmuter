#include "jpgHuffmanTree.hpp"

#include <iostream>
#include <math.h>

#include "jpgUtils.hpp"

JpgHuffmanTree::JpgHuffmanTree(std::unique_ptr<unsigned char[]> & fileData, unsigned int offSet, 
                               unsigned short segmentLenght, int & err){

    unsigned char charsWithThisLenght;
    int numCharAdded = 0;
    std::vector<std::weak_ptr<HuffmanNode>> availableNodes;
    std::shared_ptr<HuffmanNode> currentNode;
    std::shared_ptr<HuffmanNode> newNode;


    id = fileData[offSet];
    rootNode = std::make_shared<HuffmanNode>();
    rootNode->leafDepth = 0;

    nodeList.push_back(rootNode);
    availableNodes.push_back(rootNode);

    /// This loop goes through the first 16 bytes after the table ID
    /// where it is stored the number of character in each level of the tree
    for(int depth = 1; depth<17; ++depth){
        
        charsWithThisLenght = fileData[offSet + depth];

        /// Once we have the number of characters in this level we add their leaves to the tree
        for(int j = 0; j<charsWithThisLenght; ++j){
            
            currentNode = availableNodes[availableNodes.size()-1].lock();

            while(currentNode->leafDepth != depth){
                newNode = std::make_shared<HuffmanNode>();
                newNode->leafDepth = currentNode->leafDepth + 1;
                
                nodeList.push_back(newNode);

                if(currentNode->leftNode.expired()){

                    currentNode->leftNode = newNode;
                }else{
                    availableNodes.pop_back();

                    currentNode->rightNode = newNode;
                }

                availableNodes.push_back(newNode);
                currentNode = newNode;
            }


            currentNode->hasCharacter = true;
            currentNode->character = fileData[offSet+ 17 + numCharAdded];

            availableNodes.pop_back();
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


unsigned char JpgHuffmanTree::decodeChar(std::unique_ptr<unsigned char []> & scanData, 
                                         unsigned int & byteOffset, unsigned int & bitOffset, int & err){

    HuffmanNode currentNode = *rootNode;
    std::weak_ptr<HuffmanNode> nextNodePointer;
    unsigned char currentByte = scanData[byteOffset];
    unsigned char bitMultiplier = 1 << bitOffset;


    while(!currentNode.hasCharacter){

        /// This condition checks if the current bit is 0 or 1
        if(currentByte & bitMultiplier){// Bit equals 1
            nextNodePointer = currentNode.rightNode;
        }else{// Bit equals to 0
            nextNodePointer = currentNode.leftNode;
        }

        if(nextNodePointer.expired()){
            std::cerr << "ERROR: The program could not decode the huffmanCoding of the scan data\n";
            err = -1;
            return 0;
        }

        currentNode = *nextNodePointer.lock();

        if(moveBitOffset(byteOffset, bitOffset, bitMultiplier)){
            currentByte = scanData[byteOffset];
        }
    }


    return currentNode.character;

}