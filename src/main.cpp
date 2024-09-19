#include <iostream>
#include <fstream>
#include "BMPCodecFunctions.h"

using namespace std;

int main(int argc, char* argv[]){

    if(argc<2){
        cout << "Add the file name as an argument\n";
        return -1; 
    }

    fstream myFile;
    myFile.open(argv[1], ios::in | ios::binary);

    if(!myFile){
        cout << "The file " << argv[1] << " could not be opened\n";
        return -2;
    }

    Image decodedImage;

    decodeBMP(&myFile, &decodedImage);

    myFile.close();

    return 0;
}