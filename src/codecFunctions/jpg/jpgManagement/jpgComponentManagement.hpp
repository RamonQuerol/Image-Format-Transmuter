#include <vector>
#include <memory>

#include "jpgStructs.hpp"
#include "jpgEnums.hpp"

ComponentDataInfo createComponentInfo(std::unique_ptr<unsigned char []> & fileData, int offset);


void getComponentSamplingFactors(unsigned char samplingByte, Component & component);


JpgType determineJpgType(int numComponents, Component & yComponent);