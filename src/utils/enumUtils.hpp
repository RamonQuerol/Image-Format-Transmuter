#include <iostream>

#include "configEnums.hpp"

#ifndef ENUM_UTILS_H
#define ENUM_UTILS_H

//// FileFormat

int parseFileFormat(std::string preParsedText, FileFormat & parseResult);

std::string fileFormatToString(FileFormat format);

///// ColorType

int parseColorType(std::string preParsedText, ColorType & parseResult);

/// @brief Copies the tranparency configuration from fromColorType to toColorType
/// @param fromColorType Color type the transparency is copied from
/// @param toColorType Color type the transparency is copied to
void colorTypeCopyTranparency(ColorType fromColorType, ColorType & toColorType);


std::string colorTypeToString(ColorType colorType);

#endif