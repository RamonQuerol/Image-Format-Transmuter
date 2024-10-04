#include <memory>
#include "dataStructures.hpp"

/// @brief Transforms the bytes in rawPixelData into Pixels and stores them in imagePixels
/// @param rawPixelData Byte array with all the data
/// @param colorType Color type of the image (Found in the header of the png)
/// @param numPixels Total amount of pixels (height*width), also know as the size of imagePixels
/// @param imagePixels Pixel array where the translation will be stored
/// @return Return 0 if the data was correctly translated or -1 if there has been an error
int translateToPixelArray(std::unique_ptr<unsigned char[]> & rawPixelData, unsigned char colorType,
                            int numPixels, std::unique_ptr<Pixel[]> & imagePixels);