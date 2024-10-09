
/// Standard libraries
#include <memory>

/// Other files
#include "configEnums.hpp"
#include "dataStructures.hpp"


// Returns the number of bytes each pixel of the image takes based on the color type
// or -1 if the color type is not supported
int getBytesPerPixelPNG(unsigned char colorType);

// Returns the unsigned char equivalent to the enumColorType obtained from the metadata of the struct Image
// if the charColorType is not support returns UNDEFINED
unsigned char getCharColorTypePNG(ColorType enumColorType);

// Returns the enum ColorType equivalent to the charColorType obtained from the PNG header
// if the charColorType is not support returns UNDEFINED
ColorType getEnumColorTypePNG(unsigned char charColorType);