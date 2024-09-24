#include <iostream>
#include <fstream>
#include "BMPCodecFunctions.hpp"
#include "init.hpp"


int main(int argc, char* argv[]){

    if(argc<2){
        std::cout << "Add the file name as an argument\n";
        return -1; 
    }

    InitConfig config;

    if(getConfigFromFlags(argc, argv, config)){
        std::cout << "The arguments are not correctly written. Use --help to know more.\n";
        return -2;
    }

    if(config.is_info_only){
        return 0;
    }

    std::fstream inputFile;
    inputFile.open(argv[1], std::ios::in | std::ios::binary);

    if(!inputFile){
        std::cout << "The file " << argv[1] << " could not be opened\n";
        return -3;
    }

    Image decodedImage;

    int err = decodeBMP(inputFile, decodedImage);

    inputFile.close();

    std::fstream outputFile;
    outputFile.open("output.bmp", std::ios::out | std::ios::binary);

    if(!outputFile){
        std::cout << "The file "<< "outputFile" << " could not be opened\n";
        return -4;
    }  

    err = encodeBMP(outputFile, decodedImage);

    outputFile.close();

    return 0;
}