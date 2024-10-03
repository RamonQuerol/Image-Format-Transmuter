
/// Standard libraries
#include <memory>
#include <functional>

/// Other files
#include "dataStructures.hpp"


// Returns the number of bytes each pixel of the image takes based on the color type
// or -1 if the color type is not supported
int getBytesPerPixelPNG(unsigned char colorType);


// Stores in getPixelFunction a function capable extracting from a byte array 
// the pixel stored in an offset position (int value).  
// The function is chosen based on the color type of the image 
// 
// Returns 0 if the function was found and -1 if its otherwise.
int obtainGetPixelFunctionPNG(unsigned char colorType, 
                            std::function<Pixel(std::unique_ptr<unsigned char[]> &, int)> & getPixelFunction);