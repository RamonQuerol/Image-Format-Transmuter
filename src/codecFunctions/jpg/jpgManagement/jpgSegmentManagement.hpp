
#include <memory>
#include <vector>

#include "jpgStructs.hpp"

/// @brief Initializes variable with the data stored inside the header
void extractHeaderData(std::unique_ptr<unsigned char []> & fileData, unsigned int fileDataOffset,
                       unsigned char & bitsPerPixels, unsigned short & height, unsigned short & width,
                       std::vector<Component> & components, int & numComponents);


/// @brief Extracts the scan data stored in and after fileData[fileDataOffset], creates a new array of bytes and stores it in scanData. Addionaly modifies the offset and remaining values to match the next segment marker.
/// @param fileData Byte array with all the data of jpeg file
/// @param fileDataOffset Offset that points to the first byte of the scan data (After the scan data header)
/// @param remainingBytes The remaining bytes on the file
/// @param scanData An undefined pointer to a byte array where the function will return the generated array
/// @return The size of the scanData array or -1 if there was an error
int extractScanData(std::unique_ptr<unsigned char[]> & fileData,unsigned int  & fileDataOffset,
                    int & remainingBytes, std::unique_ptr<unsigned char[]> & scanData);