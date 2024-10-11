#include <iostream>

#include "configEnums.hpp"
#include "dataStructures.hpp"

#ifndef ENUM_UTILS_H
#define ENUM_UTILS_H

//// FileFormat

int parseFileFormat(std::string preParsedText, FileFormat & parseResult);

// Return the extension of the format given
std::string fileFormatToString(FileFormat format);

///// ColorType

int parseColorType(std::string preParsedText, ColorType & parseResult);

/// @brief Copies the tranparency configuration from fromColorType to toColorType
/// @param fromColorType Color type the transparency is copied from
/// @param toColorType Color type the transparency is copied to
void colorTypeCopyTranparency(ColorType fromColorType, ColorType & toColorType);

/// @brief Transforms the image pixels to their gray values using the luminosity method
/// @param imagePixels Array of Pixels that contain RGB values
/// @param numPixels Size of the array
void colorTypeColorToGray(std::unique_ptr<Pixel []> & imagePixels, int numPixels);

std::string colorTypeToString(ColorType colorType);

#endif