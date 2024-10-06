#include "init.hpp"

#include "enumUtils.hpp"


int parseFileFormat(std::string preParsedText,FileFormat & parseResult);

int getConfigFromFlags(int argc, char* argv[], InitConfig & config){

    bool transparent;

    // Initialization of config values
    config.is_info_only = false;
    config.inputFormat = UNDEFINED_FORMAT;
    config.outputFormat = UNDEFINED_FORMAT;
    config.outputColorType = UNDEFINED_COLOR;

    for(int i = 1; i < argc; ++i){
        std::string option = argv[i];
        
        // I would love to use a switch here but it seems complicated enough to not be worth it in this case
        // so let the if monolith begin

        // Flag that sets the input file format
        if(option == "-f" || option == "--from"){
            if(i+1 >= argc){
                std::cerr << "ERROR: When using " << option << " you must specify the input file format right after the flag.\n";
                return -1;
            }

            std::string preParsedFileFormat = argv[i+1];
            
            // We parse the text to the enum data type FileFormat and add it to the config
            if(parseFileFormat(preParsedFileFormat, config.inputFormat)){
                std::cerr << "ERROR: " << preParsedFileFormat << " is either incorrectly written or is not supported as a file format.\n";
                return -1;
            }

            ++i;
            continue;
        }


        // Flag that sets the output file format
        if(option == "-t" || option == "--to"){
            if(i+1 >= argc){
                std::cerr << "ERROR: When using " << option << " you must specify the output file format right after the flag.\n";
                return -1;
            }

            std::string preParsedFileFormat = argv[i+1];
            
            // We parse the text to the enum data type FileFormat and add it to the config
            if(parseFileFormat(preParsedFileFormat, config.outputFormat)){
                std::cerr << "ERROR: " << preParsedFileFormat << " is either incorrectly written or is not supported as a file format.\n";
                return -1;
            }

            ++i;
            continue;
        }


        // Flag that sets the output file name
        if(option == "-o" || option == "--output"){
            if(i+1 >= argc){
                std::cerr << "ERROR: When using " << option << " you must specify the output file name right after the flag.\n";
                return -1;
            }

            config.outputFileName = argv[i+1];
            ++i;
            continue;
        }
        

        // Flag that sets the output color type, overwriting the one from the image
        if(option == "-c" || option == "--color"){
            if(i+1 >= argc){
                std::cerr << "ERROR: When using " << option << " you must specify the output file format right after the flag.\n";
                return -1;
            }

            std::string preParsedFileFormat = argv[i+1];
            
            // We parse the text to the enum data type FileFormat and add it to the config
            if(parseColorType(preParsedFileFormat, config.outputColorType)){
                std::cerr << "ERROR: " << preParsedFileFormat << " is either incorrectly written or is not supported as a file format.\n";
                return -1;
            }

            ++i;
            continue;
        }


        // Flag that prints the current version of the program
        if(option == "--version"){
            std::cout << "ImageFormatTransmuter version 0.2\n";
            config.is_info_only = true;
            return 0;
        }


        // Flag that print useful input about the program (Like flags or supported formats)
        if(option == "-h" || option == "--help"){
            std::string helpText = "Mandatory flags:\n"
                    "\t-f, --from \tSets the input file format\n"
                    "\t-t, --to \tSets the output file format\n"
                    "\n"
                    "Optional flags:\n"
                    "\t-o, --output \tSets the output file name\n"
                    "\t-c, --color \tSets the output color type (Usually defaulted to the input color type)\n"
                    "\t    --version \tDisplays the installed version of the program and exits\n"
                    "\t-h, --help \tDisplays this text and exits\n"
                    "\n"
                    "Suported file formats:\n"
                    "\tbmp, bitmap\tBoth as output and as input\n"
                    "\tpng        \tBoth as output and as input\n"
                    "\n"
                    "Suported color types: \n"
                    "\tgray\n"
                    "\tcolor      \t It is RGB or BGR depending on the file format\n";
            
            std::cout << helpText;
            config.is_info_only = true;
            return 0;
        }

    }  


    return 0;
}
