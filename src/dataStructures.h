
// Pixel information
struct Pixel {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
};

// The metadata of the Image
struct Metadata {
    int gamma;
};

// General image data to transfer the image between decoder and ecoder
struct Image{
    int width;
    int heigth;
    int imageDataSize; // Not sure yet
    Metadata *metadata;
    Pixel *imageData;
};