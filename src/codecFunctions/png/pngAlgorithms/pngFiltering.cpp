
#include "pngFiltering.hpp"
#include <iostream>

///// Suporting methods prototypes

/// Filtering


/// Unfiltering

// All unfilter methods reverse their respective filtering from the next row of the image
// They have all the same parameters:
//
//   - filteredData: A byte array that has the filters applied to the data (This array includes the filter method at the start of every row)
//   - width: The width of the final image
//   - bytesPerPixel: Number of bytes each pixel takes
//   - inputOffset: Offset of filteredData that points to the first byte that contains data in the current row
//   - outputBuffer: The byte array where the unfiltered data is going to be dumped
//   - outputOffset: Offset of outputBuffer that points to the first byte of each row
//
// One key difference between filteredData and the outputBuffer is their size, filteredData
// has one byte more each row that contains the filtering method used.

int reverseNoneFilter(std::unique_ptr<unsigned char[]> & filteredData, unsigned int width,
                    unsigned int bytesPerPixel, int inputOffset,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset);


int reverseSubFilter(std::unique_ptr<unsigned char[]> & filteredData, unsigned int width,
                    unsigned int bytesPerPixel, int inputOffset,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset);


int reverseUpFilter(std::unique_ptr<unsigned char[]> & filteredData, unsigned int width,
                    unsigned int bytesPerPixel, int inputOffset,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset);


int reverseAverageFilter(std::unique_ptr<unsigned char[]> & filteredData, unsigned int width,
                    unsigned int bytesPerPixel, int inputOffset,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset);


int reversePaethFilter(std::unique_ptr<unsigned char[]> & filteredData, unsigned int width,
                    unsigned int bytesPerPixel, int inputOffset,
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

        switch(filterType){
            case 0:
                err = reverseNoneFilter(filteredData, width, bytesPerPixel, inputOffset, outputBuffer, outputOffset);
                break;
            case 1:
                err = reverseSubFilter(filteredData, width, bytesPerPixel, inputOffset, outputBuffer, outputOffset);
                break;
            case 2:
                err = reverseUpFilter(filteredData, width, bytesPerPixel, inputOffset, outputBuffer, outputOffset);
                break;
            case 3:
                err = reverseAverageFilter(filteredData, width, bytesPerPixel, inputOffset, outputBuffer, outputOffset);
                break;
            case 4:
                err = reversePaethFilter(filteredData, width, bytesPerPixel, inputOffset, outputBuffer, outputOffset);
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

int reverseNoneFilter(std::unique_ptr<unsigned char[]> & filteredData, unsigned int width,
                    unsigned int bytesPerPixel, int inputOffset,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    
    /// None filter means that we just have to move the data from one buffer to the other
    for(int i = 0; i<width*bytesPerPixel; ++i){
        outputBuffer[outputOffset+i] = filteredData[inputOffset+i];
    }

    return 0;
}



int reverseSubFilter(std::unique_ptr<unsigned char[]> & filteredData, unsigned int width,
                    unsigned int bytesPerPixel, int inputOffset,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    

    /// The first pixel does not have a pixel on the left so we equal it to 0
    for(int i = 0; i<bytesPerPixel; ++i){
        outputBuffer[outputOffset+i] = filteredData[inputOffset+i];
    }

    /// To unfilter the subFilter we just have to add the pixel on the left side of our current pixel
    for(int i = bytesPerPixel; i<width*bytesPerPixel; ++i){
        outputBuffer[outputOffset+i] = filteredData[inputOffset+i] + outputBuffer[outputOffset+i-bytesPerPixel];
    }

    return 0;
}



int reverseUpFilter(std::unique_ptr<unsigned char[]> & filteredData, unsigned int width,
                    unsigned int bytesPerPixel, int inputOffset,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    
    /// inputOffset == 0 means that we are on the first row and have no upper pixels
    /// We will handdle the row as if those values where 0
    if(inputOffset == 0){
        return reverseNoneFilter(filteredData, width, bytesPerPixel, inputOffset, outputBuffer, outputOffset);
    }

    int upOffset = outputOffset-width*bytesPerPixel; // Offset that points to the byte directly above the current one
    
    for(int i = 0; i<width*bytesPerPixel; ++i){
        outputBuffer[outputOffset+i] = filteredData[inputOffset+i] + outputBuffer[upOffset+i];
    }

    return 0;
}



int reverseAverageFilter(std::unique_ptr<unsigned char[]> & filteredData, unsigned int width,
                    unsigned int bytesPerPixel, int inputOffset,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    
    unsigned char average;
    int upOffset = outputOffset-width*bytesPerPixel; // Offset that points to the byte directly above the current one
    
    /// inputOffset == 0 means that we are on the first row and have no upper pixels
    /// We will handdle the row as if those values where 0
    if(inputOffset == 0){

        // First pixel of the row (No pixel on its left)
        for(int i = 0; i<bytesPerPixel; ++i){
            outputBuffer[outputOffset+i] = filteredData[inputOffset+i];
        }

        for(int i = bytesPerPixel; i<width*bytesPerPixel; ++i){

            average = calculateAverage(outputBuffer[outputOffset+i-bytesPerPixel], 0);
            outputBuffer[outputOffset+i] = filteredData[inputOffset+i] + average;
        }

        return 0;
    }

    // First pixel of the row (No pixel on its left) 
    for(int i = 0; i<bytesPerPixel; ++i){

        average = calculateAverage(0, outputBuffer[upOffset+i]);
        outputBuffer[outputOffset+i] = filteredData[inputOffset+i] + average;
    }

    for(int i = bytesPerPixel; i<width*bytesPerPixel; ++i){

        average = calculateAverage(outputBuffer[outputOffset+i-bytesPerPixel], outputBuffer[upOffset+i]);
        outputBuffer[outputOffset+i] = filteredData[inputOffset+i] + average;
    }

    return 0;
}



int reversePaethFilter(std::unique_ptr<unsigned char[]> & filteredData, unsigned int width,
                    unsigned int bytesPerPixel, int inputOffset,
                    std::unique_ptr<unsigned char[]> & outputBuffer, int outputOffset){
    
    unsigned char paeth;
    int upOffset = outputOffset-width*bytesPerPixel; // Offset that points to the byte directly above the current one


    /// inputOffset == 0 means that we are on the first row and have no upper pixels
    /// We will handdle the row as if those values where 0
    if(inputOffset == 0){

        // First pixel of the row (No pixel on its left) 
        for(int i = 0; i<bytesPerPixel; ++i){
            outputBuffer[outputOffset+i] = filteredData[inputOffset+i];
        }

        for(int i = bytesPerPixel; i<width*bytesPerPixel; ++i){

            paeth = calculatePaeth(outputBuffer[outputOffset+i-bytesPerPixel], 0, 0);
            outputBuffer[outputOffset+i] = filteredData[inputOffset+i] + paeth;
        }

        return 0;
    }



    // First pixel of the row (No pixel on its left) 
    for(int i = 0; i<bytesPerPixel; ++i){

        paeth = calculatePaeth(0, outputBuffer[upOffset+i], 0);
        outputBuffer[outputOffset+i] = filteredData[inputOffset+i] + paeth;
    }


    for(int i = bytesPerPixel; i<width*bytesPerPixel; ++i){

        paeth = calculatePaeth(outputBuffer[outputOffset+i-bytesPerPixel], outputBuffer[upOffset+i], outputBuffer[upOffset+i-bytesPerPixel]);
        outputBuffer[outputOffset+i] = filteredData[inputOffset+i] + paeth;
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
    int pLeft = abs(p-leftByte);
    int pUp = abs(p-upByte);
    int pDiag = abs(p-diagonalByte);

    // We find the minimun between pLeft, pUp and pDiag. And return the corresponding byte
    if(pLeft<=pUp && pLeft <= pDiag){
        return leftByte;
    }
    
    if(pUp <= pDiag){
        return upByte;
    }

    return diagonalByte;
}