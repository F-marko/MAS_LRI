#ifndef JPG_H_
#define JPG_H_

#include "dct.h"
#include "huffman_coding.h"

OUTPUT_STREAM* write_out(RAW_YCbCr_IMAGE* raw_ycbcr_image, unsigned short width, unsigned short height);

#endif