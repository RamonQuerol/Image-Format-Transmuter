#include "jpgPixelManagement.hpp"

#include <iostream>
#include <cstring>

/// General function prototypes

void color444ToPixels(unsigned short height, unsigned short width,
                   std::vector<JpgBlock> & yBlocks, std::vector<JpgBlock> & cbBlocks, 
                   std::vector<JpgBlock> & crBlocks, std::unique_ptr<Pixel []> & outputPixels);


void colorHorizontalToPixels(unsigned short height, unsigned short width,
                            std::vector<JpgBlock> & yBlocks, std::vector<JpgBlock> & cbBlocks, 
                            std::vector<JpgBlock> & crBlocks, std::unique_ptr<Pixel []> & outputPixels);


void colorVerticalToPixels(unsigned short height, unsigned short width,
                           std::vector<JpgBlock> & yBlocks, std::vector<JpgBlock> & cbBlocks, 
                           std::vector<JpgBlock> & crBlocks, std::unique_ptr<Pixel []> & outputPixels);


void color420ToPixels(unsigned short height, unsigned short width,
                      std::vector<JpgBlock> & yBlocks, std::vector<JpgBlock> & cbBlocks, 
                      std::vector<JpgBlock> & crBlocks, std::unique_ptr<Pixel []> & outputPixels);


void grayToPixels(unsigned short height, unsigned short width,
                  std::vector<JpgBlock> & blocks, std::unique_ptr<Pixel []> & outputPixels);

/// Individual block function prototypes

void color444BlockToPixels(unsigned short height, unsigned short width,
                        unsigned int currentHeight, unsigned int currentWidth,
                        JpgBlock & yBlock, JpgBlock & cbBlocks, JpgBlock & crBlocks,
                        std::unique_ptr<Pixel []> & outputPixels);

void colorHorizontalBlockToPixels(unsigned short height, unsigned short width,
                                  unsigned int acOffset,
                                  unsigned int currentHeight, unsigned int currentWidth,
                                  JpgBlock & yBlock, JpgBlock & cbBlock, JpgBlock & crBlock,
                                  std::unique_ptr<Pixel []> & outputPixels);


void colorVerticalBlockToPixels(unsigned short height, unsigned short width,
                                unsigned int acOffset,
                                unsigned int currentHeight, unsigned int currentWidth,
                                JpgBlock & yBlock, JpgBlock & cbBlock, JpgBlock & crBlock,
                                std::unique_ptr<Pixel []> & outputPixels);

void color420BlockToPixels(unsigned short height, unsigned short width,
                           unsigned int acHeight, unsigned int acWidth,
                           unsigned int currentHeight, unsigned int currentWidth,
                           JpgBlock & yBlock, JpgBlock & cbBlock, JpgBlock & crBlock,
                           std::unique_ptr<Pixel []> & outputPixels);


void grayBlockToPixel(unsigned short height, unsigned short width,
                      unsigned int currentHeight, unsigned int currentWidth,
                      JpgBlock & block, std::unique_ptr<Pixel []> & outputPixels);

/// Individual pixel function prototypes

Pixel yCbCrToPixel(int yData, int cbData, int crData);

/// Main Functions


void blocksToPixels(unsigned short height, unsigned short width, JpgType jpgType,
                    std::vector<Component> & components,  std::unique_ptr<Pixel []> & outputPixels){

    switch (jpgType){
    case COLOR_4_4_4:
        color444ToPixels(height, width, components[0].blocks, 
                         components[1].blocks, components[2].blocks, outputPixels);
        break;
    case COLOR_4_2_2_HORIZONTAL:
        colorHorizontalToPixels(height, width, components[0].blocks, 
                                components[1].blocks, components[2].blocks, outputPixels);
        break;
    case COLOR_4_2_2_VERTICAL:
        colorVerticalToPixels(height, width, components[0].blocks, 
                              components[1].blocks, components[2].blocks, outputPixels);
        break;
    case COLOR_4_2_0:
        color420ToPixels(height, width, components[0].blocks, 
                         components[1].blocks, components[2].blocks, outputPixels);
        break;
    case GRAY_JPG:
        grayToPixels(height, width, components[0].blocks, outputPixels);
        break;
    default:
        break;
    }

}

/// General functions

void color444ToPixels(unsigned short height, unsigned short width,
                      std::vector<JpgBlock> & yBlocks, std::vector<JpgBlock> & cbBlocks, 
                      std::vector<JpgBlock> & crBlocks, std::unique_ptr<Pixel []> & outputPixels){


    int blocksPerColumn = height/8 + (height%8 ? 1 : 0);
    int blocksPerRow = width/8 + (width%8 ? 1 : 0);

    for(int blockY = 0; blockY<blocksPerColumn; ++blockY){
        
        for(int blockX = 0; blockX<blocksPerRow; ++blockX){
            color444BlockToPixels(height, width, blockY*8, blockX*8, yBlocks[blockY*blocksPerRow + blockX], 
                cbBlocks[blockY*blocksPerRow + blockX], crBlocks[blockY*blocksPerRow + blockX], outputPixels);
        }
    }

}


void colorHorizontalToPixels(unsigned short height, unsigned short width,
                             std::vector<JpgBlock> & yBlocks, std::vector<JpgBlock> & cbBlocks, 
                             std::vector<JpgBlock> & crBlocks, std::unique_ptr<Pixel []> & outputPixels){
    
    int dcOffset;
    int acOffset;
    int acBlocksPerColumn = height/8 + (height%8 ? 1 : 0);
    int acBlocksPerRow = width/16 + (width%16 ? 1 : 0);

    for(int blockY = 0; blockY<acBlocksPerColumn; ++blockY){
        
        for(int blockX = 0; blockX<acBlocksPerRow; ++blockX){
            acOffset = blockY*acBlocksPerRow + blockX;
            dcOffset = 2*acOffset;

            colorHorizontalBlockToPixels(height, width, 0, blockY*8, blockX*16, yBlocks[dcOffset], 
                cbBlocks[acOffset], crBlocks[acOffset], outputPixels);

            colorHorizontalBlockToPixels(height, width, 4, blockY*8, blockX*16+8, yBlocks[dcOffset+1], 
                cbBlocks[acOffset], crBlocks[acOffset], outputPixels);
        }
    }
}



void colorVerticalToPixels(unsigned short height, unsigned short width,
                           std::vector<JpgBlock> & yBlocks, std::vector<JpgBlock> & cbBlocks, 
                           std::vector<JpgBlock> & crBlocks, std::unique_ptr<Pixel []> & outputPixels){

    int dcOffset;
    int acOffset;
    int acBlocksPerColumn = height/16 + (height%16 ? 1 : 0);
    int acBlocksPerRow = width/8 + (width%8 ? 1 : 0);

    for(int blockY = 0; blockY<acBlocksPerColumn; ++blockY){
        
        for(int blockX = 0; blockX<acBlocksPerRow; ++blockX){
            acOffset = blockY*acBlocksPerRow + blockX;
            dcOffset = 2*acOffset;


            colorVerticalBlockToPixels(height, width, 0, blockY*16, blockX*8, yBlocks[dcOffset], 
                cbBlocks[acOffset], crBlocks[acOffset], outputPixels);

            colorVerticalBlockToPixels(height, width, 4, blockY*16+8, blockX*8, yBlocks[dcOffset+1], 
                cbBlocks[acOffset], crBlocks[acOffset], outputPixels);
        }
    }
}


void color420ToPixels(unsigned short height, unsigned short width,
                      std::vector<JpgBlock> & yBlocks, std::vector<JpgBlock> & cbBlocks, 
                      std::vector<JpgBlock> & crBlocks, std::unique_ptr<Pixel []> & outputPixels){

    int dcOffset;
    int acOffset;
    int acBlocksPerColumn = height/16 + (height%16 ? 1 : 0);
    int acBlocksPerRow = width/16 + (width%16 ? 1 : 0);

    for(int blockY = 0; blockY<acBlocksPerColumn; ++blockY){
        
        for(int blockX = 0; blockX<acBlocksPerRow; ++blockX){
            acOffset = blockY*acBlocksPerRow + blockX;
            dcOffset = 4*acOffset;

            color420BlockToPixels(height, width, 0, 0, blockY*16, blockX*16, yBlocks[dcOffset], 
                cbBlocks[acOffset], crBlocks[acOffset], outputPixels);

            color420BlockToPixels(height, width, 0, 4, blockY*16, blockX*16+8, yBlocks[dcOffset+1], 
                cbBlocks[acOffset], crBlocks[acOffset], outputPixels);

            color420BlockToPixels(height, width, 4, 0, blockY*16+8, blockX*16, yBlocks[dcOffset+2], 
                cbBlocks[acOffset], crBlocks[acOffset], outputPixels);

            color420BlockToPixels(height, width, 4, 4, blockY*16+8, blockX*16+8, yBlocks[dcOffset+3], 
                cbBlocks[acOffset], crBlocks[acOffset], outputPixels);
        }
    }
}


void grayToPixels(unsigned short height, unsigned short width,
                  std::vector<JpgBlock> & blocks, std::unique_ptr<Pixel []> & outputPixels){

    int blocksPerColumn = height/8 + (height%8 ? 1 : 0);
    int blocksPerRow = width/8 + (width%8 ? 1 : 0);

    for(int blockY = 0; blockY<blocksPerColumn; ++blockY){
        
        for(int blockX = 0; blockX<blocksPerRow; ++blockX){
            grayBlockToPixel(height, width, blockY*8, blockX*8, 
                            blocks[blockY*blocksPerRow + blockX], outputPixels);
        }
    }

}

/// Individual block funcitions

void color444BlockToPixels(unsigned short height, unsigned short width,
                           unsigned int currentHeight, unsigned int currentWidth,
                           JpgBlock & yBlock, JpgBlock & cbBlock, JpgBlock & crBlock,
                           std::unique_ptr<Pixel []> & outputPixels){
                            
    unsigned int heightOffset = currentHeight;
    unsigned int widthOffset = currentWidth;
    unsigned int blockOffset = 0;

    for(int i = 0; i<8 && heightOffset<height; ++i, ++heightOffset){
        for(int j = 0; j<8 && widthOffset<width; ++j, ++widthOffset, ++blockOffset){
            outputPixels[heightOffset*width+widthOffset] = 
            yCbCrToPixel(yBlock.blockData[blockOffset], cbBlock.blockData[blockOffset], 
                         crBlock.blockData[blockOffset]);
        }
        widthOffset = currentWidth;
    }
}

void colorHorizontalBlockToPixels(unsigned short height, unsigned short width,
                                   unsigned int acOffset,
                                   unsigned int currentHeight, unsigned int currentWidth,
                                   JpgBlock & yBlock, JpgBlock & cbBlock, JpgBlock & crBlock,
                                   std::unique_ptr<Pixel []> & outputPixels){
                            
    unsigned int heightOffset = currentHeight;
    unsigned int widthOffset = currentWidth;

    for(int i = 0; i<8 && heightOffset<height; ++i, ++heightOffset){
        for(int j = 0; j<8 && widthOffset<width; ++j, ++widthOffset){

            outputPixels[heightOffset*width+widthOffset] = 
            yCbCrToPixel(yBlock.blockData[i*8+j], cbBlock.blockData[i*8+j/2+acOffset], 
                        crBlock.blockData[i*8+j/2+acOffset]);
        }
        widthOffset = currentWidth;
    }
}


void colorVerticalBlockToPixels(unsigned short height, unsigned short width,
                                unsigned int acOffset,
                                unsigned int currentHeight, unsigned int currentWidth,
                                JpgBlock & yBlock, JpgBlock & cbBlock, JpgBlock & crBlock,
                                std::unique_ptr<Pixel []> & outputPixels){
                            
    unsigned int heightOffset = currentHeight;
    unsigned int widthOffset = currentWidth;

    for(int i = 0; i<8 && heightOffset<height; ++i, ++heightOffset){
        for(int j = 0; j<8 && widthOffset<width; ++j, ++widthOffset){

            outputPixels[heightOffset*width+widthOffset] = 
            yCbCrToPixel(yBlock.blockData[i*8+j], cbBlock.blockData[i*4 + 8*acOffset + j], 
                        crBlock.blockData[i*4 + 8*acOffset + j]);
        }
        widthOffset = currentWidth;
    }
}


void color420BlockToPixels(unsigned short height, unsigned short width,
                           unsigned int acHeight, unsigned int acWidth,
                           unsigned int currentHeight, unsigned int currentWidth,
                           JpgBlock & yBlock, JpgBlock & cbBlock, JpgBlock & crBlock,
                           std::unique_ptr<Pixel []> & outputPixels){
                            
    unsigned int heightOffset = currentHeight;
    unsigned int widthOffset = currentWidth;

    for(int i = 0; i<8 && heightOffset<height; ++i, ++heightOffset){
        for(int j = 0; j<8 && widthOffset<width; ++j, ++widthOffset){

            outputPixels[heightOffset*width+widthOffset] = 
            yCbCrToPixel(yBlock.blockData[i*8+j], cbBlock.blockData[i*4 + 8*acHeight + j/2 + acWidth], 
                        crBlock.blockData[i*4 + 8*acHeight + j/2 + acWidth]);
        }
        widthOffset = currentWidth;
    }
}


void grayBlockToPixel(unsigned short height, unsigned short width,
                      unsigned int currentHeight, unsigned int currentWidth,
                      JpgBlock & block, std::unique_ptr<Pixel []> & outputPixels){
                            
    Pixel pixel;
    int grayValue;
    unsigned char grayByte;
    unsigned int heightOffset = currentHeight;
    unsigned int widthOffset = currentWidth;
    unsigned int blockOffset = 0;

    pixel.alpha = 255;

    for(int i = 0; i<8 && heightOffset<height; ++i, ++heightOffset){
        for(int j = 0; j<8 && widthOffset<width; ++j, ++widthOffset, ++blockOffset){
            
            grayValue = block.blockData[blockOffset] +128;

            if(grayValue < 0){
                grayByte = 0;
            }else if(grayValue > 255){
                grayByte = 255;
            }else{
                grayByte = grayValue;
            }

            memset(&pixel, grayByte, 3);
            
            outputPixels[heightOffset*width+widthOffset] = pixel;
        }
        widthOffset = currentWidth;
    }

}

/// Pixel functions

Pixel yCbCrToPixel(int yData, int cbData, int crData){

    Pixel pixel;
    int redInt;
    int greenInt;
    int blueInt;

    pixel.alpha = 255;

    redInt =   yData                   + 1.402*crData    + 128;
    greenInt = yData - 0.344136*cbData - 0.714136*crData + 128;
    blueInt =  yData + 1.772*cbData                      + 128;


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

    return pixel;
}