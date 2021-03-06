#ifndef JPG_H_
#define JPG_H_

#include "dct.h"
#include "huffman_coding.h"

OUTPUT_STREAM* make_jpg_image(RAW_YCbCr_IMAGE* raw_ycbcr_image, unsigned short width, unsigned short height);

#endif