

#ifndef CONFIG_ENUMS_H
#define CONFIG_ENUMS_H

enum FileFormat {
    BITMAP,
    PNG,
    JPG,
    UNDEFINED_FORMAT
};

enum ColorType {
    GRAY,
    GRAY_WITH_ALPHA,
    COLOR,
    COLOR_WITH_ALPHA,
    UNDEFINED_COLOR
};

#endif