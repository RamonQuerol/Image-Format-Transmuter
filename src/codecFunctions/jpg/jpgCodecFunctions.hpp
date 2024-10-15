// Standard libraries
#include <fstream>

// Other files
#include "dataStructures.hpp"

int encodeJPG(std::fstream & outputFile, Image & image);

int decodeJPG(std::fstream & inputFile, Image & decodedImage);