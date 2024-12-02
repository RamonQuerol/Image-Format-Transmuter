
#include <memory>
#include <vector>

#include "jpgStructs.hpp"


/// Segments
#define START_SEGEMENT_MARKER 55551 /// FF D8
#define APP0_SEGMENT_MARKER 57599 /// FF E0
#define APP1_SEGMENT_MARKER 57855 // FF E1
#define QUANTIZATION_SEGMENT_MARKER 56319 // FF DB
#define BASELINE_SEGMENT_MARKER 49407 // FF C0
#define PROGRESSIVE_SEGEMENT_MARKER 49919 /// FF C2
#define HUFFMAN_SEGMENT_MARKER 50431 /// FF C4
#define SCAN_SEGMENT_MARKER 56063 /// FF DA
#define DEFINE_RESTART_MARKER 56831 // FF DD
#define COMMENT_SEGMENT_MARKER 65279 // FF FE
#define END_SEGMENT_MARKER 55807 /// FF D9


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
                    bool & usesRestartMarkers,  int & remainingBytes, 
                    std::unique_ptr<unsigned char[]> & scanData);