#include "jpgSegmentManagement.hpp"

#include <iostream>
#include <cstring>


#include "jpgComponentManagement.hpp"
#include "fileDataManagementUtils.hpp"

void extractHeaderData(std::unique_ptr<unsigned char []> & fileData, unsigned int fileDataOffset,
                       unsigned char & bitsPerPixels, unsigned short & height, unsigned short & width,
                       std::vector<Component> & components, int & numComponents){
    
    Component tempComponent;
    
    bitsPerPixels = fileData[fileDataOffset];
    height = extractBigEndianUshort(fileData, fileDataOffset+1);
    width = extractBigEndianUshort(fileData, fileDataOffset +3);

    numComponents = fileData[fileDataOffset + 5];

    for(int i = 0; i<numComponents; ++i){
        tempComponent.componentID = fileData[fileDataOffset+6+i*3]-1;
        getComponentSamplingFactors(fileData[fileDataOffset + 7 + i*3], tempComponent);
        tempComponent.quatizationTable = fileData[fileDataOffset + 8 + i*3];
        
        components.push_back(tempComponent);
    }
}


int extractScanData(std::unique_ptr<unsigned char[]> & fileData,unsigned int  & fileDataOffset,
                    int & remainingBytes, std::unique_ptr<unsigned char[]> & scanData){
    
    // When copying the scanData is important to take to account that since it ends when it reaches 
    // the next segment marker (which always starts with FF), the actual FF values in scan data have
    // an special marker (FF 00). 
    // In order to store the correct data we have to convert all the FF 00 into just FF.

    unsigned char *startingPointer = fileData.get()+fileDataOffset;
    unsigned char *supportPointer = startingPointer;
    unsigned int checkedBytes = 0;
    bool endOfScanData = false;
    int uncheckedBytes = remainingBytes;

    int numOfFF = 0;
    
    std::unique_ptr<std::vector<int>> checkHistory = std::make_unique<std::vector<int>>();
    int startCopyOffset;
    int nextCopySize;

    // The first loop finds the size scanData and know where all the FF 00 are
    while(!endOfScanData && uncheckedBytes>1){
        supportPointer = reinterpret_cast<unsigned char *>(memchr(supportPointer, 255, uncheckedBytes));
        
        // A nullptr means that it has not found a FF in the remaining bytes which is impossible
        // because it has to at least find the end of frame marker (FF D9)
        if(supportPointer == nullptr){
            std::cerr << "The input file does not end with the jpg end of frame marker and therefore can not be decoded\n";
            return -1;
        }

        checkedBytes = supportPointer-startingPointer;

        uncheckedBytes = remainingBytes - checkedBytes;
        checkHistory->push_back(checkedBytes);
        
        ++supportPointer;
        ++numOfFF;

        if(supportPointer[0] != 0){
            endOfScanData = true;
        }

    }

    /// Now that we can calculate the actual size of the scanData we can create the array
    scanData = std::make_unique<unsigned char[]>(checkedBytes-numOfFF+1);
    startCopyOffset = 0;

    /// Once we have scanned the scan data we can copy the data to the new array
    for(int i = 0; i<numOfFF-1; ++i){
        nextCopySize = checkHistory->at(i)-startCopyOffset+1;

        memcpy(scanData.get()+startCopyOffset-i, startingPointer+startCopyOffset, nextCopySize);

        startCopyOffset = checkHistory->at(i)+2;
    }

    nextCopySize = checkHistory->at(numOfFF-1)-startCopyOffset;
    memcpy(scanData.get()+startCopyOffset-numOfFF+1, startingPointer+startCopyOffset, nextCopySize);

    remainingBytes = uncheckedBytes;
    fileDataOffset += checkedBytes;

    return checkedBytes-numOfFF+1;
}