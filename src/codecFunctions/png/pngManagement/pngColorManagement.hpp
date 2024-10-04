
/// Standard libraries
#include <memory>

/// Other files
#include "dataStructures.hpp"


// Returns the number of bytes each pixel of the image takes based on the color type
// or -1 if the color type is not supported
int getBytesPerPixelPNG(unsigned char colorType);
