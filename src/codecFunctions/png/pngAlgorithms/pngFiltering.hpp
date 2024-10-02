#include <memory>

int unfilterPNG(std::unique_ptr<unsigned char[]> filteredData, unsigned int height, 
                unsigned int width, unsigned int bytesPerPixel, 
                std::unique_ptr<unsigned char[]> & outputBuffer);