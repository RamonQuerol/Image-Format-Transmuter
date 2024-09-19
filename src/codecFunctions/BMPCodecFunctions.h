// General libraries
#include <fstream>

// Other files
#include "dataStructures.h"

using namespace std;

void encodeBMP(Image image, fstream *outputFile);

int decodeBMP(fstream *inputFile, Image *decodedImage);