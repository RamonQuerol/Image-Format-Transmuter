// General libraries
#include <fstream>

// Other files
#include "dataStructures.h"

void encodeBMP(Image image, std::fstream *outputFile);

int decodeBMP(std::fstream *inputFile, Image *decodedImage);