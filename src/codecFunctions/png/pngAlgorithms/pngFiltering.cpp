
#include "pngFiltering.hpp"
#include <iostream>
#include <cstring>

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
    
    int bytesPerRow = width*bytesPerPixel;
    int inputOffset = 0;
    int outputOffset = 0;
    int err = 0;
    unsigned char filterType;

    /// The for loop will go row by row unfiltering the data with the corresponding method
    /// and storing it in the outputBuffer
    for(int i = 0; i<height; ++i){
        
        /// In filteredData the first byte of each row defines the filter type
        filterType = filteredData[inputOffset];
        ++inputOffset;
        
        memcpy(outputBuffer.get()+outputOffset, filteredData.get()+inputOffset, bytesPerRow);


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
                std::cerr << "The png input image has a unknown filter type\n";
                return -1;
        }

        if(err){
            return -1;
        }

        // Although inputOffset has one more byte per row, we have alredy added that byte before the switch statement
        inputOffset += bytesPerRow;
        outputOffset += bytesPerRow;
    }
   

    return 0;
}


//// Suporting methods

/// Unfiltering suport methods


int reverseSubFilter(unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    

    /// The first pixel does not have a pixel on the left so we equal it to 0
    // for(int i = 0; i<bytesPerPixel; ++i){
    //     outputBuffer[outputOffset+i] = filteredData[inputOffset+i];
    // }

    /// To unfilter the subFilter we just have to add the pixel on the left side of our current pixel
    for(int i = bytesPerPixel; i<bytesPerRow; ++i){
        outputBuffer[outputOffset+i] += outputBuffer[outputOffset+i-bytesPerPixel];
    }

    return 0;
}



int reverseUpFilter(unsigned int bytesPerPixel, unsigned int bytesPerRow,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    
    /// outputOffset == 0 means that we are on the first row and have no upper pixels
    /// We will handdle the row as if those values where 0
    if(outputOffset == 0){
        return 0;
    }

    int upOffset = outputOffset-bytesPerRow; // Offset that points to the byte directly above the current one
    
    for(int i = 0; i<bytesPerRow; ++i){
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
    if(outputOffset == 0){

        // First pixel of the row (No pixel on its left)
        // for(int i = 0; i<bytesPerPixel; ++i){
        //     outputBuffer[outputOffset+i] = filteredData[inputOffset+i];
        // }

        for(int i = bytesPerPixel; i<bytesPerRow; ++i){

            average = calculateAverage(outputBuffer[outputOffset+i-bytesPerPixel], 0);
            outputBuffer[outputOffset+i] += average;
        }

        return 0;
    }

    // First pixel of the row (No pixel on its left) 
    for(int i = 0; i<bytesPerPixel; ++i){

        average = calculateAverage(0, outputBuffer[upOffset+i]);
        outputBuffer[outputOffset+i] += average;
    }

    for(int i = bytesPerPixel; i<bytesPerRow; ++i){

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
    if(outputOffset == 0){

        // First pixel of the row (No pixel on its left) 
        // for(int i = 0; i<bytesPerPixel; ++i){
        //     outputBuffer[outputOffset+i] = filteredData[inputOffset+i];
        // }

        for(int i = bytesPerPixel; i<bytesPerRow; ++i){

            paeth = outputBuffer[outputOffset+i-bytesPerPixel];
            outputBuffer[outputOffset+i] += paeth;
        }

        return 0;
    }



    // First pixel of the row (No pixel on its left) 
    for(int i = 0; i<bytesPerPixel; ++i){

        paeth = outputBuffer[upOffset+i];
        outputBuffer[outputOffset+i] += paeth;
    }


    for(int i = bytesPerPixel; i<bytesPerRow; ++i){

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
    
    int p = leftByte + upByte - diagonalByte;
    
    // Calculate the distance between p and all the other points
    int pLeft = p-leftByte;
    int pUp = p-upByte;
    int pDiag = p-diagonalByte;

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