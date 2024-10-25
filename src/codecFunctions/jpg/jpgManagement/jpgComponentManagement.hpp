#include <vector>

#include "jpgStructs.hpp"
#include "jpgEnums.hpp"

void assignHuffTablesToComponent(unsigned char huffTableByte, Component & component);


void getComponentSamplingFactors(unsigned char samplingByte, Component & component);


JpgType determineJpgType(int numComponents, Component & yComponent);