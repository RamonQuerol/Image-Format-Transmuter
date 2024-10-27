


#ifndef JPG_ENUMS_H
#define JPG_ENUMS_H

/// @brief Defines the encoding of the image
enum JpgEncoding{
    UNDEFINED_JPG_ENCODING,
    BASELINE_ENCODING,
    PROGRESSIVE_ENCODING
};


/// @brief Defines the color and subsampling of the jpg image
enum JpgType{
    GRAY_JPG,
    COLOR_4_4_4,
    COLOR_4_2_2_VERTICAL,
    COLOR_4_2_2_HORIZONTAL,
    COLOR_4_2_0
};

#endif