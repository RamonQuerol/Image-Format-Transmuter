
/// Standard libraries 
#include <fstream>

/// Other files
#include "dataStructures.hpp"

#define GRAY_8BIT_COLOR_TABLE_SIZE 1024

// Returns the bytesPerPixel based on the colorType, 
// in case of an unknown colortype it returns the bytes of an opaque color image (3 bytes)
int bmpColorTypeToBytesPerPixel(ColorType colorType);

// Creates the color table needed for 8bits gray images and adds it to the outputFile
void bmpAdd8BitsColorTable(std::fstream & outputFile);