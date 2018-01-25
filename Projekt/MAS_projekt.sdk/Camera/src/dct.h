#ifndef DCT_H_
#define DCT_H_

#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI	3.14

static unsigned char K1[8][8] = {
	{ 16, 11, 10, 16, 24, 40, 51, 61 },
	{ 12, 12, 14, 19, 26, 58, 60, 55 },
	{ 14, 13, 16, 24, 40, 57, 69, 56 },
	{ 14, 17, 22, 29, 51, 87, 80, 62 },
	{ 18, 22, 37, 56, 68, 109, 103, 77 },
	{ 24, 35, 55, 64, 81, 104, 113, 92 },
	{ 49, 64, 78, 87, 103, 121, 120, 101 },
	{ 72, 92, 95, 98, 112, 100, 103, 99 }
};

static unsigned char K2[8][8] = {
	{ 17, 18, 24, 47, 99, 99, 99, 99 },
	{ 18, 21, 26, 66, 99, 99, 99, 99 },
	{ 24, 26, 56, 99, 99, 99, 99, 99 },
	{ 47, 66, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99 },
	{ 99, 99, 99, 99, 99, 99, 99, 99 }
};

static unsigned char zig_zag[64] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };

static float cos_lookup_table[8][8] = {
	{ 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000 },
	{ 0.980805, 0.831635, 0.555984, 0.195774, -0.194212, -0.554659, -0.830750, -0.980493 },
	{ 0.923956, 0.383235, -0.381764, -0.923345, -0.924564, -0.384706, 0.380291, 0.922733 },
	{ 0.831635, -0.194212, -0.980493, -0.557307, 0.553334, 0.981421, 0.198896, -0.828973 },
	{ 0.707388, -0.706262, -0.708513, 0.705133, 0.709636, -0.704003, -0.710757, 0.702871 },
	{ 0.555984, -0.980493, 0.192649, 0.833400, -0.828973, -0.200457, 0.982027, -0.549347 },
	{ 0.383235, -0.924564, 0.922733, -0.378818, -0.387644, 0.926374, -0.920880, 0.374391 },
	{ 0.195774, -0.557307, 0.833400, -0.981725, 0.979543, -0.827187, 0.548016, -0.184829 }
};

typedef struct pixel_ycbcr{
	short Y;
	short Cb;
	short Cr;
}PIXEL_YCbCr;

typedef struct raw_ycbcr_image{
	PIXEL_YCbCr ycbcr[480][640];
}RAW_YCbCr_IMAGE;

typedef struct ycbcr_block{
	PIXEL_YCbCr ycbcr[8][8];
}YCbCr_BLOCK;

void read_ycbcr_block(RAW_YCbCr_IMAGE* raw_ycbcr_image, int A, int B, YCbCr_BLOCK* ycbcr_block);
void write_ycbcr_block(RAW_YCbCr_IMAGE* raw_ycbcr_image, int A, int B, YCbCr_BLOCK* ycbcr_block);

void shift_ycbcr_block(YCbCr_BLOCK* ycbcr_block);
void block_dct(YCbCr_BLOCK* ycbcr_block, YCbCr_BLOCK* result);
void quantize_ycbcr_block(YCbCr_BLOCK* ycbcr_block);
void dct(RAW_YCbCr_IMAGE* raw_ycbcr_image, unsigned short width, unsigned short height);

void init_raw_ycbcr_image(RAW_YCbCr_IMAGE* raw_ycbcr_image, unsigned short width, unsigned short height);
void free_raw_ycbcr_image(RAW_YCbCr_IMAGE* raw_ycbcr_image, unsigned short width, unsigned short height);
#endif
