#include <memory>

#include "dataStructures.hpp"

#ifndef PIXEL_UTILS_H
#define PIXEL_UTILS_H

Pixel grayToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int offSet);

Pixel grayWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int offSet);

Pixel rgbToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int offSet);

Pixel rgbWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int offSet);

#endif