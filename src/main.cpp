#include <iostream>
#include <fstream>

#include "init.hpp"
#include "configEnums.hpp"
#include "dataStructures.hpp"

#include "enumUtils.hpp"

#include "bmpCodecFunctions.hpp"
#include "pngCodecFunctions.hpp"


int main(int argc, char* argv[]){

    if(argc<2){
        std::cerr << "Add the file name as an argument\n";
        return -1; 
    }

    InitConfig config;
    Image decodedImage;
    std::string outputName;
    std::string formatExtension;
    int err = -1;


    if(getConfigFromFlags(argc, argv, config)){
        std::cerr << "The arguments are not correctly written. Use --help to know more.\n";
        return -2;
    }

    if(config.is_info_only){
        return 0;
    }

    std::fstream inputFile;
    inputFile.open(argv[1], std::ios::in | std::ios::binary);

    if(!inputFile){
        std::cerr << "The file " << argv[1] << " could not be opened\n";
        return -3;
    }


    switch(config.inputFormat){
        case BITMAP:
            err = decodeBMP(inputFile, decodedImage);
            break;
        case PNG:
            err = decodePNG(inputFile, decodedImage);
            break;
        case UNDEFINED_FORMAT:
            std::cerr << "You must define the input file format. Use --help to know more.\n";
            return -4;
        default:
            std::cerr << "The current version of the program does not support " + fileFormatToString(config.inputFormat) <<
                    " as an input format.\n";
            return -5;
    }
    
    inputFile.close();
    
    if(err){
        std::cerr << "Something went wrong during the decodification of the image\n";
        return -6;
    }

    //// Output flags effects

    // Output color type
    if(config.outputColorType != UNDEFINED_COLOR){
        colorTypeCopyTranparency(decodedImage.metadata.colorType, config.outputColorType);
        decodedImage.metadata.colorType = config.outputColorType;
    }

    // Output name

    formatExtension = fileFormatToString(config.outputFormat);

    if(formatExtension==fileFormatToString(UNDEFINED_FORMAT)){
        std::cerr << "The program does not know the extension for the output format you gave\n";
    }

    if(config.outputFileName != ""){
        outputName = config.outputFileName + "." + formatExtension;
    }else{
        outputName = "output." + formatExtension;
    }


    ///// New image encoding

    std::fstream outputFile;
    outputFile.open(outputName, std::ios::out | std::ios::binary);

    if(!outputFile){
        std::cerr << "The file "<< outputName << " could not be opened\n";
        return -4;
    }  

    switch(config.outputFormat){
        case BITMAP:
            err = encodeBMP(outputFile, decodedImage);
            break;
        case PNG:
            err = encodePNG(outputFile, decodedImage);
            break;
        case UNDEFINED_FORMAT:
            outputFile.close();
            std::cerr << "You must define the output file format. Use --help to know more.\n";
            return -4;
        default:
            outputFile.close();
            std::cerr << "The current version of the program does not support " + fileFormatToString(config.outputFormat) <<
                    " as an output format.\n";
            return -5;
    }
    
    outputFile.close();

    if(err){
        std::cerr << "Something went wrong during the creation of the new image\n";
        return -6;
    }

    return 0;
}