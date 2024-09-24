#include "init.hpp"

int getConfigFromFlags(int argc, char* argv[], InitConfig & config){

    // Initialization of config values
    config.is_info_only = false;

    for(int i = 1; i < argc; ++i){
        std::string option = argv[i];
        
        // I would love to use a switch here but it seems complicated enough to not be worth it in this case
        // so let the if monolith begin

        // Flag that sets the input file format
        if(option == "-f" || option == "--from"){
            if(i+1 < argc){
                config.inputFormat = argv[i+1];
                ++i;
                continue;
            }else{
                std::cout << "ERROR: When using " << option << " you must specify the input file format right after the flag.\n";
                return -1;
            }
        }


        // Flag that sets the output file format
        if(option == "-t" || option == "--to"){
            if(i+1 < argc){
                config.outputFormat = argv[i+1];
                ++i;
                continue;
            }else{
                std::cout << "ERROR: When using " << option << " you must specify the output file format right after the flag.\n";
                return -1;
            }
        }


        // Flag that sets the output file name
        if(option == "-o" || option == "--output"){
            if(i+1 < argc){
                config.outputFileName = argv[i+1];
                ++i;
                continue;
            }else{
                std::cout << "ERROR: When using " << option << " you must specify the output file name right after the flag.\n";
                return -1;
            }
        }
        
        
        // Flag that prints the current version of the program
        if(option == "--version"){
            std::cout << "ImageFormatTransmuter version 0.1\n";
            config.is_info_only = true;
            return 0;
        }


        // Flag that print useful input about the program (Like flags or supported formats)
        if(option == "--h" || option == "--help"){
            std::string helpText = "Mandatory flags:\n"
                    "\t-f, --from \tSets the input file format\n"
                    "\t-t, --to \tSets the output file format\n"
                    "\n"
                    "Optional flags:\n"
                    "\t-o, --output \tSets the output file name\n"
                    "\t    --version \tDisplays the installed version of the program and exits\n"
                    "\t-h, --help \tDisplays this text and exits\n";
            
            std::cout << helpText;
            config.is_info_only = true;
            return 0;
        }

    }  


    return 0;
}