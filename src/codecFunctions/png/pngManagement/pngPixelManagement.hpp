
/// Standard libraries 
#include <memory>

/// Other files
#include "dataStructures.hpp"
#include "configEnums.hpp"


/// @brief Transforms the bytes in rawPixelData into Pixels and stores them in imagePixels
/// @param imagePixels Pixel array with all the data
/// @param colorType Color type of the image (Found in the metadata of the decoded image)
/// @param numPixels Total amount of pixels (height*width), also know as the size of imagePixels
/// @param rawPixelData Byte array where the translation will be stored (Its size depends on the color type)
/// @return Return 0 if the data was correctly translated or -1 if there has been an error
int translateToByteArray(std::unique_ptr<Pixel[]> imagePixels, ColorType colorType,
                            int numPixels, std::unique_ptr<unsigned char[]> & rawPixelData);

/// @brief Transforms the bytes in rawPixelData into Pixels and stores them in imagePixels
/// @param rawPixelData Byte array with all the data
/// @param colorType Color type of the image (Found in the header of the png)
/// @param numPixels Total amount of pixels (height*width), also know as the size of imagePixels
/// @param imagePixels Pixel array where the translation will be stored
/// @return Return 0 if the data was correctly translated or -1 if there has been an error
int translateToPixelArray(std::unique_ptr<unsigned char[]> rawPixelData, unsigned char colorType,
                            int numPixels, std::unique_ptr<Pixel[]> & imagePixels);