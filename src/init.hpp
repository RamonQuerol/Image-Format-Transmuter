#include <iostream>

enum FileFormat {
    BITMAP,
    PNG,
    JPG,
    UNDEFINED
};

// Stores the initial configuration from the flags
struct InitConfig{
    FileFormat inputFormat;
    FileFormat outputFormat;
    std::string outputFileName;
    bool is_info_only; // If this is true the execution should stop (since its for flags that print info)
};

int getConfigFromFlags(int argc, char* argv[], InitConfig & config);

std::string fileFormatToString(FileFormat format);