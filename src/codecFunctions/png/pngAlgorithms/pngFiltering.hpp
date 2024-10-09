#include <memory>

int filterPNG(std::unique_ptr<unsigned char[]> rawData, unsigned int height,
                unsigned int width, unsigned int bytesPerPixel,
                std::unique_ptr<unsigned char[]> & filteredData);


int unfilterPNG(std::unique_ptr<unsigned char[]> filteredData, unsigned int height, 
                unsigned int width, unsigned int bytesPerPixel, 
                std::unique_ptr<unsigned char[]> & outputBuffer);