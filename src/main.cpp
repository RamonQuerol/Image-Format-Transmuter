#include <iostream>
#include <fstream>
#include "BMPCodecFunctions.h"


int main(int argc, char* argv[]){

    if(argc<2){
        std::cout << "Add the file name as an argument\n";
        return -1; 
    }

    std::fstream inputFile;
    inputFile.open(argv[1], std::ios::in | std::ios::binary);

    if(!inputFile){
        std::cout << "The file " << argv[1] << " could not be opened\n";
        return -2;
    }

    Image decodedImage;

    int err = decodeBMP(inputFile, decodedImage);

    // for(int i = 0; i<decodedImage.heigth*decodedImage.width; ++i){
    //     std::cout << decodedImage.imageData[i].blue-0 << " " << decodedImage.imageData[i].green-0 << " " << decodedImage.imageData[i].red-0 << "\n";
    // }

    inputFile.close();

    std::fstream outputFile;
    outputFile.open("output.bmp", std::ios::out | std::ios::binary);

    if(!outputFile){
        std::cout << "The file "<< "outputFile" << " could not be opened\n";
        return -2;
    }  

    err = encodeBMP(outputFile, decodedImage);

    outputFile.close();

    return 0;
}