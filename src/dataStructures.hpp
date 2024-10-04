#include <memory>


#ifndef DATASTRUCTURES_H
#define DATASTRUCTURES_H

// Pixel information
struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
}; // Its important that this struct mantains this order since this files depend on it:
   //        - pngPixelManagement (memcpy and memset)

// The metadata of the Image
struct Metadata {
    int gamma;
};

// General image data to transfer the image between decoder and ecoder
struct Image{
    int width;
    int heigth;
    Metadata *metadata;
    // The imageData array has a size of width*heigth pixels
    std::unique_ptr<Pixel[]> imageData; // image pixels ordered from left to right and top to bottom.
};


#endif