#include <memory>

#include "dataStructures.hpp"


///// ENCODING /////


void pixelRowToGrayBMP(std::unique_ptr<Pixel[]> & imagePixels, unsigned int width,
                            unsigned int imgPixelsOffset,unsigned int rawDataOffset, 
                            std::unique_ptr<unsigned char[]> & rawImageData);



void pixelRowToColorBMP(std::unique_ptr<Pixel[]> & imagePixels, unsigned int width, unsigned int bytesPerPixel,
                            unsigned int imagePixelsInitOffset,unsigned int rawDataOffset, 
                            std::unique_ptr<unsigned char[]> & rawImageData);


///// DECODING /////


std::unique_ptr<Pixel[]> decodeColorImgDataBMP(unsigned int width, unsigned int height, 
                std::unique_ptr<unsigned char[]> & rawImageData, unsigned int bitsPerPixel,
                ColorType & colorType);


std::unique_ptr<Pixel[]> decodeGrayImageDataBMP(unsigned int width, unsigned int height, 
                std::unique_ptr<unsigned char[]> & rawImageData, unsigned int bitsPerPixel);