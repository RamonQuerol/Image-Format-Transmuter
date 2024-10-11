

#include "dataStructures.hpp"


// Returns the bytesPerPixel based on the colorType, 
// in case of an unknown colortype it returns the bytes of an opaque color image (3 bytes)
int bmpColorTypeToBytesPerPixel(ColorType colorType);