#include <memory>

#include "dataStructures.hpp"


///// ENCODING /////

void add24bitPixelToRawImageData(Pixel pixel, std::unique_ptr<unsigned char[]> & rawImageData, int offSet);


///// DECODING /////


std::unique_ptr<Pixel[]> decodeColorImgDataBMP(unsigned int width, unsigned int height, 
                std::unique_ptr<unsigned char[]> & rawImageData, unsigned int bitsPerPixel,
                ColorType & colorType);


std::unique_ptr<Pixel[]> decodeGrayImageDataBMP(unsigned int width, unsigned int height, 
                std::unique_ptr<unsigned char[]> & rawImageData, unsigned int bitsPerPixel);