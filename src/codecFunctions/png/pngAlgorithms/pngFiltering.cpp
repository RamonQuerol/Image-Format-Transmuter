
#include "pngFiltering.hpp"
#include <iostream>
#include <cstring>
#include <thread>
#include <functional>

///// Global variables

int filterError;

///// Thread Functions prototypes

void unfilterThread(std::unique_ptr<unsigned char[]> & filteredData, unsigned int height, 
                    unsigned int width, unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    int threadOffset, std::unique_ptr<unsigned char[]> & outputBuffer);

///// Suporting methods prototypes

/// Filtering


/// Unfiltering

// All unfilter methods reverse their respective filtering from the next row of the image
// They have all the same parameters:
//
//   - bytesPerPixel: Number of bytes each pixel takes
//   - bytesPerRow: Number of bytes each row takes
//   - outputBuffer: The byte array where the unfiltered data is going to be dumped
//   - outputOffset: Offset of outputBuffer that points to the first byte of each row

int reverseSubFilter(unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset);


int reverseUpFilter(unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset);


int reverseAverageFilter(unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset);


int reversePaethFilter(unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset);

/// Calculation methods

unsigned char calculateAverage(unsigned int leftByte, unsigned int upByte);

unsigned char calculatePaeth(unsigned int leftByte, unsigned int upByte, unsigned diagonalByte);


///// Main methods 

int unfilterPNG(std::unique_ptr<unsigned char[]> filteredData, unsigned int height, 
                unsigned int width, unsigned int bytesPerPixel, 
                std::unique_ptr<unsigned char[]> & outputBuffer){
    
    std::unique_ptr<std::thread[]> threads = std::make_unique<std::thread[]>(bytesPerPixel);
    int bytesPerRow = width*bytesPerPixel;
    

    filterError = 0; 

    /// Copy the data from one array to another so we just have to use one of them (and also because that way we can use memcpy)
    for(int i = 1, offset = 0; i<=height; ++i, offset += bytesPerRow){
        memcpy(outputBuffer.get()+offset, filteredData.get()+offset+i, bytesPerRow);
    }

    /// Start the threads, each thread will unfilter one color channel: red, green, blue, alpha etc...
    // We can't use more threads because unfiltering is a sequential process
    for(int i = 0; i<bytesPerPixel; ++i){
        threads[i] = std::thread(unfilterThread, std::ref(filteredData), height, width, bytesPerPixel, 
                                bytesPerRow, i, std::ref(outputBuffer));
    }

    /// Wait until all the threads have finished
    for(int i = 0; i<bytesPerPixel; ++i){
        threads[i].join();
    }

    return filterError;
}


//// Thread functions

void unfilterThread(std::unique_ptr<unsigned char[]> & filteredData, unsigned int height, 
                    unsigned int width, unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    int threadOffset, std::unique_ptr<unsigned char[]> & outputBuffer){

    int err = 0;
    int inputOffset = 0;
    int outputOffset = threadOffset; /// Set the output offset to their respective color channel
    unsigned char filterType;

    /// The for loop will go row by row unfiltering the data with the corresponding method
    /// and storing it in the outputBuffer
    for(int i = 0; i<height; ++i){
        
        /// In filteredData the first byte of each row defines the filter type
        filterType = filteredData[inputOffset];

        switch(filterType){
            case 0: // None filter
                break;
            case 1: // Sub filter
                err = reverseSubFilter(bytesPerPixel, bytesPerRow, outputBuffer, outputOffset);
                break;
            case 2: // Up filter
                err = reverseUpFilter(bytesPerPixel, bytesPerRow, outputBuffer, outputOffset);
                break;
            case 3: // Average filter
                err = reverseAverageFilter(bytesPerPixel, bytesPerRow, outputBuffer, outputOffset);
                break;
            case 4: // Paeth filter
                err = reversePaethFilter(bytesPerPixel, bytesPerRow, outputBuffer, outputOffset);
                break;
            default:
                filterError = -1;
                std::cerr << "The png input image has a unknown filter type\n";
                return;
        }

        if(err){
            filterError = -1;
            return;
        }

        // Since filteredData has the filter method on the first byte of each row
        // we have to add it to its offset
        inputOffset += bytesPerRow + 1;
        outputOffset += bytesPerRow;
    }
        
}

//// Suporting methods

/// Unfiltering suport methods


int reverseSubFilter(unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    

    /// The first pixel does not have a pixel on the left so we don't have to anything with it

    unsigned char prev = outputBuffer[outputOffset];

    /// To unfilter with subFilter we just have to add the pixel on the left side of our current pixel
    for(int i = bytesPerPixel; i<bytesPerRow; i += bytesPerPixel){

        outputBuffer[outputOffset+i] += prev;

        prev = outputBuffer[outputOffset+i];
    }

    return 0;
}



int reverseUpFilter(unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    
    /// outputOffset == 0 means that we are on the first row and have no upper pixels
    /// We will handdle the row as if those values where 0 and therefore no calculation is needed
    if(outputOffset < bytesPerPixel){
        return 0;
    }

    int upOffset = outputOffset-bytesPerRow; // Offset that points to the byte directly above the current one
    
    for(int i = 0; i<bytesPerRow; i += bytesPerPixel){
        outputBuffer[outputOffset+i] += outputBuffer[upOffset+i];
    }

    return 0;
}



int reverseAverageFilter(unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    
    unsigned char average;
    int upOffset = outputOffset-bytesPerRow; // Offset that points to the byte directly above the current one
    
    /// outputOffset == 0 means that we are on the first row and have no upper pixels
    /// We will handdle the row as if those values where 0
    if(outputOffset < bytesPerPixel){

        /// The first pixel does not have a pixel on the left nor above so we don't have to anything with it

        for(int i = bytesPerPixel; i<bytesPerRow; i += bytesPerPixel){

            average = calculateAverage(outputBuffer[outputOffset+i-bytesPerPixel], 0);
            outputBuffer[outputOffset+i] += average;
        }

        return 0;
    }

    // First pixel of the row (No pixel on its left) 
    average = calculateAverage(0, outputBuffer[upOffset]);
    outputBuffer[outputOffset] += average;

    for(int i = bytesPerPixel; i<bytesPerRow; i += bytesPerPixel){

        average = calculateAverage(outputBuffer[outputOffset+i-bytesPerPixel], outputBuffer[upOffset+i]);
        outputBuffer[outputOffset+i] += average;
    }

    return 0;
}



int reversePaethFilter(unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    
    unsigned char paeth;
    int upOffset = outputOffset-bytesPerRow; // Offset that points to the byte directly above the current one
    

    /// outputOffset == 0 means that we are on the first row and have no upper pixels
    /// We will handdle the row as if those values where 0
    if(outputOffset < bytesPerPixel){

        /// The first pixel does not have a pixel on the left nor above so we don't have to anything with it

        for(int i = bytesPerPixel; i<bytesPerRow; i += bytesPerPixel){

            paeth = outputBuffer[outputOffset+i-bytesPerPixel];
            outputBuffer[outputOffset+i] += paeth;
        }

        return 0;
    }


    // First pixel of the row (No pixel on its left) 
    paeth = outputBuffer[upOffset];
    outputBuffer[outputOffset] += paeth;


    for(int i = bytesPerPixel; i<bytesPerRow; i += bytesPerPixel){

        paeth = calculatePaeth(outputBuffer[outputOffset+i-bytesPerPixel], outputBuffer[upOffset+i], outputBuffer[upOffset+i-bytesPerPixel]);

        outputBuffer[outputOffset+i] += paeth;
    }


    return 0;
}


/// Calculation methods

unsigned char calculateAverage(unsigned int leftByte, unsigned int upByte){
    return (leftByte+upByte)/2;
}

unsigned char calculatePaeth(unsigned int leftByte, unsigned int upByte, unsigned diagonalByte){
    
    // The calculation bellow do this:
    // p = leftByte + upByte - diagonalByte;
    
    // pLeft = abs(p-leftByte)
    // pUp = abs(p-upByte)
    // pDiag = abs(p-diagonalByte)

    // Calculate the distance between p and all the other points
    int pLeft = upByte - diagonalByte; // p - leftByte == leftByte -leftByte + upByte - diagonalByte
    int pUp = leftByte - diagonalByte; // p - upByte == leftByte + upByte - upByte - diagonalByte
    int pDiag = pLeft+pUp; // p - diagonalByte == leftByte + upByte - 2*diagonalByte == (upByte - diagonalByte) + (leftByte - diagonalByte)

    pLeft = pLeft>0 ? pLeft : -pLeft;
    pUp = pUp>0 ? pUp : -pUp;
    pDiag = pDiag>0 ? pDiag : -pDiag;

    // We find the minimun between pLeft, pUp and pDiag. And return the corresponding byte
    if(pLeft<=pUp && pLeft <= pDiag){
        return leftByte;
    }
    
    if(pUp <= pDiag){
        return upByte;
    }

    return diagonalByte;
}