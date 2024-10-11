
/// Standard libraries 
#include <memory>
#include <vector>

// Compresses a bytes inside of decompressedData, and dumps the information inside of compressedData (In blocks of 32768 bytes)
// 
// Params:      
// - decompressedData: char array that stores the decompressed data after i has been filtered
// - decomDataSize: The total size of decompressedData
// - compressedData: Vector of byte buffers where all the data will be dumped after been compressed. (Each element of the vecto is an IDAT chunk)
// - compressedDataLenghtss: Vector with all the leghts of the byte buffers from the compressed data
//
// Returns: 0 if everything is ok or -1 if there was an error during decompression
int compressPNG(std::unique_ptr<unsigned char[]> decompressedData, unsigned int decomDataSize,
                std::unique_ptr<std::vector<std::unique_ptr<unsigned char[]>>> & compressedData,
                std::unique_ptr<std::vector<unsigned int>> & compressedDataLenghts);


// Decompresses a bytes inside of compressedData, and dumps the information into a char array named decompressedData (Of size decomDataSize)
// 
// Params:      
// - compressedData: Vector of byte buffers with the data of all the IDAT chunks.
// - lenghts: Vector with all the leghts of the IDAT chunk from the compressed data
// - decompressedData: char array that stores the decompressed data from all IDAT chunks
// - decomDataSize: The total size of decompressedData
//
// Returns: 0 if everything is ok or -1 if there was an error during decompression
int decompressPNG(std::unique_ptr<std::vector<std::unique_ptr<unsigned char[]>>> compressedData, 
                  std::unique_ptr<std::vector<unsigned int>> lenghts, std::unique_ptr<unsigned char[]> &decompressedData, 
                  unsigned int decomDataSize);