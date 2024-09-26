// General libraries
#include <fstream>

// Other files
#include "dataStructures.hpp"

int encodePNG(std::fstream & outputFile, Image & image);

int decodePNG(std::fstream & inputFile, Image & decodedImage);