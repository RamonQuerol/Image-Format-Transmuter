

#ifndef JPG_UTILS_H
#define JPG_UTILS_H


// Moves the byte and bit offsets 1 bit.
// Returns true if the byteOffset has been changed and false if it has not
bool moveBitOffset(unsigned int & byteOffset , unsigned int & bitOffset, unsigned char & bitMultiplier);

#endif