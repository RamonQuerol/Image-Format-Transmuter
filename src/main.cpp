#include <iostream>
#include <fstream>
#include "BMPCodecFunctions.h"


int main(int argc, char* argv[]){

    if(argc<2){
        std::cout << "Add the file name as an argument\n";
        return -1; 
    }

    std::fstream myFile;
    myFile.open(argv[1], std::ios::in | std::ios::binary);

    if(!myFile){
        std::cout << "The file " << argv[1] << " could not be opened\n";
        return -2;
    }

    Image decodedImage;

    int err = decodeBMP(myFile, decodedImage);

    for(int i = 0; i<decodedImage.heigth*decodedImage.width; ++i){
        std::cout << decodedImage.imageData[i].blue-0 << " " << decodedImage.imageData[i].green-0 << " " << decodedImage.imageData[i].red-0 << "\n";
    }

    myFile.close();

    return 0;
}