#include "jpgPixelManagement.hpp"

#include <iostream>

/// Individual block funcitions prototypes

void yCbCrBlockToPixels(unsigned short height, unsigned short width,
                        unsigned int currentHeight, unsigned int currentWidth,
                        JpgBlock & yBlock, JpgBlock & cbBlocks, JpgBlock & crBlocks,
                        std::unique_ptr<Pixel []> & outputPixels);

/// Main Functions

void yCbCrToPixels(unsigned short height, unsigned short width,
                   std::vector<JpgBlock> & yBlocks, std::vector<JpgBlock> & cbBlocks, std::vector<JpgBlock> & crBlocks,
                   std::unique_ptr<Pixel []> & outputPixels){

    int blocksPerColumn = height/8 + (height%8 ? 1 : 0);
    int blocksPerRow = width/8 + (width%8 ? 1 : 0);

    for(int i = 0; i<blocksPerColumn; ++i){
        
        for(int j = 0; j<blocksPerRow; ++j){
            yCbCrBlockToPixels(height, width, i*8, j*8, yBlocks[i*blocksPerRow + j], 
                cbBlocks[i*blocksPerRow + j], crBlocks[i*blocksPerRow + j], outputPixels);
        }
    }

}

/// Individual block funcitions

void yCbCrBlockToPixels(unsigned short height, unsigned short width,
                        unsigned int currentHeight, unsigned int currentWidth,
                        JpgBlock & yBlock, JpgBlock & cbBlocks, JpgBlock & crBlocks,
                        std::unique_ptr<Pixel []> & outputPixels){

    Pixel pixel;
    int redInt;
    int greenInt;
    int blueInt;
    unsigned int heightOffset = currentHeight;
    unsigned int widthOffset = currentWidth;

    pixel.alpha = 255;

    for(int i = 0; i<8 && heightOffset<height; ++i, ++heightOffset){
        for(int j = 0; j<8 && widthOffset<width; ++j, ++widthOffset){
            redInt =   yBlock.blockData[i*8+j]                                     + 1.402*crBlocks.blockData[i*8+j] + 128;
            greenInt = yBlock.blockData[i*8+j] - 0.344136*cbBlocks.blockData[i*8+j] -0.714136*crBlocks.blockData[i*8+j] + 128;
            blueInt =  yBlock.blockData[i*8+j] + 1.772*cbBlocks.blockData[i*8+j] + 128;


            /// RED
            if(redInt < 0){
                pixel.red = 0;
            }else if(redInt > 255){
                pixel.red = 255;
            }else{
                pixel.red = redInt;
            }

            /// GREEN        
            if(greenInt < 0){
                pixel.green = 0;
            }else if(greenInt > 255){
                pixel.green = 255;
            }else{
                pixel.green = greenInt;
            }

            /// BLUE
            if(blueInt < 0){
                pixel.blue = 0;
            }else if(blueInt > 255){
                pixel.blue = 255;
            }else{
                pixel.blue = blueInt;
            }

            outputPixels[heightOffset*width+widthOffset] = pixel;
        }

        widthOffset = currentWidth;
    
    }
}