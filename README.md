# Image-Format-Transmuter
Image Format Transmuter is a command-line tool (CLI) tool for converting images between file formats.

It currently supports the following formats:
- **PNG** (As input and output)
- **BMP** (As input and output)

This project was made as a learning exercise to improve my C++ skills and gain insights into various formats and their associated algorithms. Consequently, I am implementing as much of the codecs as possible, with the exception of those algorithms whose implementation are either well-standarized (e.g., CRC) or could make for their own project due to their complexity (e.g., deflate/inflate).

**Note:** The program was developed with Linux as the target operating system. As a result, it may or may not work on Windows or macOS computers.

## Table of contents
- [User guide](#user-guide)
- [Building the project](#building-the-project)
- [Required libraries](#required-libraries)
- [Future plans](#future-plans)

## User guide
To use the program, run the executable in the CLI with the following structure:
```sh
./imageTransmuter "input file name" -f "input file format" -t "output file format"
```
### Flags
**Mandatory flags:**

&ensp; -f, &ensp;--from &ensp; &emsp; &ensp;Sets the input file format

&ensp;  -t, &ensp;--to &emsp; &emsp; &emsp; Sets the output file format

**Optional flags:**

&ensp;-o, &ensp;--output &emsp; &nbsp;Sets the output file name

&ensp;-c, &ensp;--color &emsp; &emsp; Sets the output color type (Usually defaulted to the input color type)

&emsp; &ensp; &ensp;--version &emsp; Displays the installed version of the program and exits

&ensp;-h, &ensp;--help &emsp; &emsp;&nbsp; Displays information about the flags and accepted formats/colors

## Building the project
The project uses CMake for its build system, so it can basically be build anywhere on the system. However, I personally recommend creating a new directory to keep the build files organized.
```sh
mkdir build
cd build
```
Once inside the building directory, run `cmake` pointing to these project base directory.
```sh
cmake "path/to/Image-Format-Transmuter"
```
Finally, you compile the project using the `make` command in the building directory.
```sh
make
```

## Required libraries
- zlib (Tested with version 1.3)

## Future plans
**Image Format Transmuter** its currently in version 0.3.0.

To reach version 1.0.0, it should have support for at least the PNG, JPG and BMP formats, both for encoding and decoding. With special emphasis in the decoding process, since it should allow any images with these extensions.

#### Key features currently missing:
- JPG format (Both for encoding and decoding)
- Decoding PNG images with interlacing.

#### Planed features after version 1.0.0:
- Support for more formats  (WebP, TIFF, QOI, etc.)
- Encoding PNG images with interlacing
- Config files and additional flag options
- Using OpenGL or BLAS libraries to optimize PNG encoding
