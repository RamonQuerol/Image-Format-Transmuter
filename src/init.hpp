#include <iostream>

// Stores the initial configuration from the flags
struct InitConfig{
    std::string inputFormat;
    std::string outputFormat;
    std::string outputFileName;
    bool is_info_only; // If this is true the execution should stop (since its for flags that print info)
};

int getConfigFromFlags(int argc, char* argv[], InitConfig & config);