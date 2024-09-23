// General libraries
#include <fstream>

// Other files
#include "dataStructures.h"

int encodeBMP(std::fstream & outputFile, Image & image);

int decodeBMP(std::fstream & inputFile, Image & decodedImage);