#include "pixelUtils.hpp"


Pixel grayToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int offSet){
    Pixel pixel;

    // In RGB/BGR gray values are obtained by havinf the same value in red, green and blue
    pixel.red = rawImageData[offSet];
    pixel.green = rawImageData[offSet];
    pixel.blue = rawImageData[offSet];
    pixel.alpha = 255; // Since we don't have alpha, we will put maximun alpha value (fully opaque)

    return pixel;
}

Pixel grayWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int offSet){
    Pixel pixel;

    // In RGB/BGR gray values are obtained by havinf the same value in red, green and blue
    pixel.red = rawImageData[offSet];
    pixel.green = rawImageData[offSet];
    pixel.blue = rawImageData[offSet];

    pixel.alpha = rawImageData[offSet+1];

    return pixel;
}

Pixel rgbToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int offSet){
    Pixel pixel;

    pixel.red = rawImageData[offSet];
    pixel.green = rawImageData[offSet + 1];
    pixel.blue = rawImageData[offSet + 2];
    pixel.alpha = 255; // Since we don't have alpha, we will put maximun alpha value (fully opaque)

    return pixel;
}


Pixel rgbWithAlphaToPixel(std::unique_ptr<unsigned char[]> & rawImageData, int offSet){
    Pixel pixel;

    pixel.red = rawImageData[offSet];
    pixel.green = rawImageData[offSet + 1];
    pixel.blue = rawImageData[offSet + 2];
    pixel.alpha = rawImageData[offSet + 3];

    return pixel;
}

