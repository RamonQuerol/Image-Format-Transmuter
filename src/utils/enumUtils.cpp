#include "enumUtils.hpp"

#include <algorithm>


///// FileFormat

int parseFileFormat(std::string preParsedText, FileFormat & parseResult){

    std::string lowerCaseText = preParsedText;
    
    // We tansform the string into lowerCase
    std::transform(lowerCaseText.begin(), lowerCaseText.end(), lowerCaseText.begin(), ::tolower);

    if(lowerCaseText == "bmp" || lowerCaseText == "bitmap"){
        parseResult = BITMAP;
        return 0;
    }

    if(lowerCaseText == "png"){
        parseResult = PNG;
        return 0;
    }

    if(lowerCaseText == "jpg"){
        parseResult = JPG;
        return 0;
    }

    return -1;
}


std::string fileFormatToString(FileFormat format){
    switch(format){
        case BITMAP:
            return "Bitmap";
        case PNG:
            return "png";
        case JPG:
            return "jpg";
    }

    return "???";
}


///// ColorType

int parseColorType(std::string preParsedText, ColorType & parseResult){

    std::string lowerCaseText = preParsedText;
    
    // We tansform the string into lowerCase
    std::transform(lowerCaseText.begin(), lowerCaseText.end(), lowerCaseText.begin(), ::tolower);

    if(lowerCaseText == "color" || lowerCaseText == "colour"){
        parseResult = COLOR;
        return 0;
    }
    
    if(lowerCaseText == "gray" || lowerCaseText == "grayimage"){
        parseResult = GRAY;
        return 0;
    }

    return -1;
}

void colorTypeCopyTranparency(ColorType fromColorType, ColorType & toColorType){

    if(fromColorType == GRAY_WITH_ALPHA || fromColorType == COLOR_WITH_ALPHA){
        
        /// There is not need to check for GRAY_WITH_ALPHA or COLOR_WITH_ALPHA because they
        /// already have transparency activated

        switch(toColorType){
            case COLOR:
                toColorType = COLOR_WITH_ALPHA;
                break;
            case GRAY:
                toColorType = GRAY_WITH_ALPHA;
                break;
        }

    }else{

        /// There is not need to check for GRAY or COLOR because they are already
        /// configured to not have tranparency

        switch(toColorType){
            case COLOR_WITH_ALPHA:
                toColorType = COLOR;
                break;
            case GRAY_WITH_ALPHA:
                toColorType = GRAY;
                break;
        }
    }
}

std::string colorTypeToString(ColorType colorType){
    switch(colorType){
        case GRAY:
            return "gray";
        case GRAY_WITH_ALPHA:
            return "gray with transparency";
        case COLOR:
            return "color";
        case COLOR_WITH_ALPHA:
            return "color with transparency";
    }

    return "???";
}