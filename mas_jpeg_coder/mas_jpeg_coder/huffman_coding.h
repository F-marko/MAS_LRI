#ifndef HUFFMAN_CODING_H_
#define HUFFMAN_CODING_H_

#include <stdlib.h>
#include <string.h>
#include <math.h>

// ukupna velièina svih dodatnih podataka koje zahtjeva jpeg format
static unsigned short jpeg_format_data_size = 625;

// markeri
static unsigned char soi[] = { 0xff, 0xd8 };
static unsigned char ext[] = { 0xff, 0xe0 };
static unsigned char dqt[] = { 0xff, 0xdb };
static unsigned char dht[] = { 0xff, 0xc4 };
static unsigned char sof[] = { 0xff, 0xc0 };
static unsigned char sos[] = { 0xff, 0xda };
static unsigned char eoi[] = { 0xff, 0xd9 };

static unsigned char ext_header[] = {
	0x00, 0x10,		// duljina headera			(16 bajtova)
	0x4a, 0x46, 0x49, 0x46,		//				"JFIF"
	0x00, 0x01,		// jfif major version
	0x01, 0x02,		// jfif minor version

	0x00, 0x1c,
	0x00, 0x1c,

	0x00,
	0x00
};

static unsigned char dqt_lum_ctx[] = {
	0x00, 0x43,		// duljina					(67)

	0x00,			// preciznost/odredište		(8 bit / 0 luminance)
	16, 11, 12, 14, 12, 10, 16, 14, 13, 14, 18, 17, 16, 19, 24, 40, 26, 24, 22, 22, 24, 49, 35, 37, 29, 40, 58, 51, 61, 60, 57, 51, 56, 55, 64, 72, 92, 78, 64, 68, 87, 69, 55, 56, 80, 109, 81, 87, 95, 98, 103, 104, 103, 62, 77, 113, 121, 112, 100, 120, 92, 101, 103, 99,
};

static unsigned char dqt_chr_ctx[] = {
	0x00, 0x43,		// duljina					(67)

	0x01,			// preciznost/odredište		(8 bit / 1 chrominance)
	17, 18, 18, 24, 21, 24, 47, 26, 26, 47, 99, 66, 56, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
};

static unsigned char frame_header[] = {
	0x00, 0x11,		// duljina frame headdera	(17 bajtova)
	0x08,			// preciznost uzoraka		(8 bit)
	/***********************************************************************/
	/*
	0x02, 0x00,		// broj linija				(512)
	0x02, 0x00,		// broj uzoraka u liniji	(512)
	*/
	0x01, 0xe0,		// broj linija				(480)
	0x02, 0x80,		// broj uzoraka u liniji	(640)
	/***********************************************************************/
	0x03,			// broj komponenti			(Y, Cb, Cr)

	0x01,			// identifikator komponente
	0x11,			// h/v sampling factor
	0x00,			// odredište kvantizacijske tablice	(luminance q table)

	0x02,			// identifikator komponente
	0x11,			// h/v sampling factor
	0x01,			// odredište kvantizacijske tablice	(chrominance q table)

	0x03,			// identifikator komponente
	0x11,			// h/v sampling factor
	0x01,			// odredište kvantizacijske tablice (chrominance q table)
};

static unsigned char huffman_table_dc_luminance[] = {
	0x00, 0x1f,		// velièina (31 bajtova)
	0x00,			// 0 = DC, odredište = 0
	// broj kodava po duljinama koda
	0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,

	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

static unsigned char huffman_table_ac_luminance[] = {
	0x00, 0xb5,		// valièina	(181 bajt)
	0x10,			// 1 = AC, odredište = 0
	// broj kodava po duljinama koda
	0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 125,
	// run/length po vrijednostima kodova
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
};

static unsigned char huffman_table_dc_chrominance[] = {
	0x00, 0x1f,		// velièina (31 bajtova)
	0x01,			// 0 = DC, odredište = 1
	// broj kodava po duljinama koda
	0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,

	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

static unsigned char huffman_table_ac_chrominance[] = {
	0x00, 0xb5,		// valièina	(181 bajt)
	0x11,			// 1 = AC, odredište = 1
	// broj kodava po duljinama koda
	0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 119,
	// run/length po vrijednostima kodova
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
};

static unsigned char scan_header[] = {
	0x00, 0x0c,		// duljina scan headera		(12 bajtova)
	0x03,			// broj komponenti us scanu

	0x01,			// identifikator komponente
	0x00,			// odreiste za ad i dc tablice, za luminance su u 0

	0x02,			// identifikator komponente
	0x11,			// odreiste za ad i dc tablice, za chrominance su u 1

	0x03,			// identifikator komponente
	0x11,			// odreiste za ad i dc tablice, za chrominance su u 1

	0x00,			// Ss	(0)
	0x3f,			// Se	(63)
	0x00
};

static unsigned short dc_luminance[12][2] = {
	{ 2, 0 },
	{ 3, 2 },
	{ 3, 3 },
	{ 3, 4 },
	{ 3, 5 },
	{ 3, 6 },
	{ 4, 14 },
	{ 5, 30 },
	{ 6, 62 },
	{ 7, 126 },
	{ 8, 254 },
	{ 9, 510 },
};

static unsigned short dc_chrominance[12][2] = {
	{ 2, 0 },
	{ 2, 1 },
	{ 2, 2 },
	{ 3, 6 },
	{ 4, 14 },
	{ 5, 30 },
	{ 6, 62 },
	{ 7, 126 },
	{ 8, 254 },
	{ 9, 510 },
	{ 10, 1022 },
	{ 11, 2046 },
};

static unsigned short ac_eob_lum[2] = {
	4, 10
};

static unsigned short ac_zrl_lum[2] = {
	11, 2041
};

static unsigned short ac_eob_chrom[2] = {
	2, 0
};

static unsigned short ac_zrl_chrom[2] = {
	10, 1018
};

// AC kodovi za Y komponente bez EOB i ZRL kodova
static unsigned short ac_luminance[160][2] = {
	{ 2, 0 },
	{ 2, 1 },
	{ 3, 4 },
	{ 4, 11 },
	{ 5, 26 },
	{ 7, 120 },
	{ 8, 248 },
	{ 10, 1014 },
	{ 16, 65410 },
	{ 16, 65411 },
	{ 4, 12 },
	{ 5, 27 },
	{ 7, 121 },
	{ 9, 502 },
	{ 11, 2038 },
	{ 16, 65412 },
	{ 16, 65413 },
	{ 16, 65414 },
	{ 16, 65415 },
	{ 16, 65416 },
	{ 5, 28 },
	{ 8, 249 },
	{ 10, 1015 },
	{ 12, 4084 },
	{ 16, 65417 },
	{ 16, 65418 },
	{ 16, 65419 },
	{ 16, 65420 },
	{ 16, 65421 },
	{ 16, 65422 },
	{ 6, 58 },
	{ 9, 503 },
	{ 12, 4085 },
	{ 16, 65423 },
	{ 16, 65424 },
	{ 16, 65425 },
	{ 16, 65426 },
	{ 16, 65427 },
	{ 16, 65428 },
	{ 16, 65429 },
	{ 6, 59 },
	{ 10, 1016 },
	{ 16, 65430 },
	{ 16, 65431 },
	{ 16, 65432 },
	{ 16, 65433 },
	{ 16, 65434 },
	{ 16, 65435 },
	{ 16, 65436 },
	{ 16, 65437 },
	{ 7, 122 },
	{ 11, 2039 },
	{ 16, 65438 },
	{ 16, 65439 },
	{ 16, 65440 },
	{ 16, 65441 },
	{ 16, 65442 },
	{ 16, 65443 },
	{ 16, 65444 },
	{ 16, 65445 },
	{ 7, 123 },
	{ 12, 4086 },
	{ 16, 65446 },
	{ 16, 65447 },
	{ 16, 65448 },
	{ 16, 65449 },
	{ 16, 65450 },
	{ 16, 65451 },
	{ 16, 65452 },
	{ 16, 65453 },
	{ 8, 250 },
	{ 12, 4087 },
	{ 16, 65454 },
	{ 16, 65455 },
	{ 16, 65456 },
	{ 16, 65457 },
	{ 16, 65458 },
	{ 16, 65459 },
	{ 16, 65460 },
	{ 16, 65461 },
	{ 9, 504 },
	{ 15, 32704 },
	{ 16, 65462 },
	{ 16, 65463 },
	{ 16, 65464 },
	{ 16, 65465 },
	{ 16, 65466 },
	{ 16, 65467 },
	{ 16, 65468 },
	{ 16, 65469 },
	{ 9, 505 },
	{ 16, 65470 },
	{ 16, 65471 },
	{ 16, 65472 },
	{ 16, 65473 },
	{ 16, 65474 },
	{ 16, 65475 },
	{ 16, 65476 },
	{ 16, 65477 },
	{ 16, 65478 },
	{ 9, 506 },
	{ 16, 65479 },
	{ 16, 65480 },
	{ 16, 65481 },
	{ 16, 65482 },
	{ 16, 65483 },
	{ 16, 65484 },
	{ 16, 65485 },
	{ 16, 65486 },
	{ 16, 65487 },
	{ 10, 1017 },
	{ 16, 65488 },
	{ 16, 65489 },
	{ 16, 65490 },
	{ 16, 65491 },
	{ 16, 65492 },
	{ 16, 65493 },
	{ 16, 65494 },
	{ 16, 65495 },
	{ 16, 65496 },
	{ 10, 1018 },
	{ 16, 65497 },
	{ 16, 65498 },
	{ 16, 65499 },
	{ 16, 65500 },
	{ 16, 65501 },
	{ 16, 65502 },
	{ 16, 65503 },
	{ 16, 65504 },
	{ 16, 65505 },
	{ 11, 2040 },
	{ 16, 65506 },
	{ 16, 65507 },
	{ 16, 65508 },
	{ 16, 65509 },
	{ 16, 65510 },
	{ 16, 65511 },
	{ 16, 65512 },
	{ 16, 65513 },
	{ 16, 65514 },
	{ 16, 65515 },
	{ 16, 65516 },
	{ 16, 65517 },
	{ 16, 65518 },
	{ 16, 65519 },
	{ 16, 65520 },
	{ 16, 65521 },
	{ 16, 65522 },
	{ 16, 65523 },
	{ 16, 65524 },
	{ 16, 65525 },
	{ 16, 65526 },
	{ 16, 65527 },
	{ 16, 65528 },
	{ 16, 65529 },
	{ 16, 65530 },
	{ 16, 65531 },
	{ 16, 65532 },
	{ 16, 65533 },
	{ 16, 65534 }
};

// AC kodovi za Cb i Cr komponente bez EOB i ZRL kodova
static unsigned short ac_chrominance[160][2] = {
	{ 2, 1 },
	{ 3, 4 },
	{ 4, 10 },
	{ 5, 24 },
	{ 5, 25 },
	{ 6, 56 },
	{ 7, 120 },
	{ 9, 500 },
	{ 10, 1014 },
	{ 12, 4084 },
	{ 4, 11 },
	{ 6, 57 },
	{ 8, 246 },
	{ 9, 501 },
	{ 11, 2038 },
	{ 12, 4085 },
	{ 16, 65416 },
	{ 16, 65417 },
	{ 16, 65418 },
	{ 16, 65419 },
	{ 5, 26 },
	{ 8, 247 },
	{ 10, 1015 },
	{ 12, 4086 },
	{ 15, 32706 },
	{ 16, 65420 },
	{ 16, 65421 },
	{ 16, 65422 },
	{ 16, 65423 },
	{ 16, 65424 },
	{ 5, 27 },
	{ 8, 248 },
	{ 10, 1016 },
	{ 12, 4087 },
	{ 16, 65425 },
	{ 16, 65426 },
	{ 16, 65427 },
	{ 16, 65428 },
	{ 16, 65429 },
	{ 16, 65430 },
	{ 6, 58 },
	{ 9, 502 },
	{ 16, 65431 },
	{ 16, 65432 },
	{ 16, 65433 },
	{ 16, 65434 },
	{ 16, 65435 },
	{ 16, 65436 },
	{ 16, 65437 },
	{ 16, 65438 },
	{ 6, 59 },
	{ 10, 1017 },
	{ 16, 65439 },
	{ 16, 65440 },
	{ 16, 65441 },
	{ 16, 65442 },
	{ 16, 65443 },
	{ 16, 65444 },
	{ 16, 65445 },
	{ 16, 65446 },
	{ 7, 121 },
	{ 11, 2039 },
	{ 16, 65447 },
	{ 16, 65448 },
	{ 16, 65449 },
	{ 16, 65450 },
	{ 16, 65451 },
	{ 16, 65452 },
	{ 16, 65453 },
	{ 16, 65454 },
	{ 7, 122 },
	{ 11, 2040 },
	{ 16, 65455 },
	{ 16, 65456 },
	{ 16, 65457 },
	{ 16, 65458 },
	{ 16, 65459 },
	{ 16, 65460 },
	{ 16, 65461 },
	{ 16, 65462 },
	{ 8, 249 },
	{ 16, 65463 },
	{ 16, 65464 },
	{ 16, 65465 },
	{ 16, 65466 },
	{ 16, 65467 },
	{ 16, 65468 },
	{ 16, 65469 },
	{ 16, 65470 },
	{ 16, 65471 },
	{ 9, 503 },
	{ 16, 65472 },
	{ 16, 65473 },
	{ 16, 65474 },
	{ 16, 65475 },
	{ 16, 65476 },
	{ 16, 65477 },
	{ 16, 65478 },
	{ 16, 65479 },
	{ 16, 65480 },
	{ 9, 504 },
	{ 16, 65481 },
	{ 16, 65482 },
	{ 16, 65483 },
	{ 16, 65484 },
	{ 16, 65485 },
	{ 16, 65486 },
	{ 16, 65487 },
	{ 16, 65488 },
	{ 16, 65489 },
	{ 9, 505 },
	{ 16, 65490 },
	{ 16, 65491 },
	{ 16, 65492 },
	{ 16, 65493 },
	{ 16, 65494 },
	{ 16, 65495 },
	{ 16, 65496 },
	{ 16, 65497 },
	{ 16, 65498 },
	{ 9, 506 },
	{ 16, 65499 },
	{ 16, 65500 },
	{ 16, 65501 },
	{ 16, 65502 },
	{ 16, 65503 },
	{ 16, 65504 },
	{ 16, 65505 },
	{ 16, 65506 },
	{ 16, 65507 },
	{ 11, 2041 },
	{ 16, 65508 },
	{ 16, 65509 },
	{ 16, 65510 },
	{ 16, 65511 },
	{ 16, 65512 },
	{ 16, 65513 },
	{ 16, 65514 },
	{ 16, 65515 },
	{ 16, 65516 },
	{ 14, 16352 },
	{ 16, 65517 },
	{ 16, 65518 },
	{ 16, 65519 },
	{ 16, 65520 },
	{ 16, 65521 },
	{ 16, 65522 },
	{ 16, 65523 },
	{ 16, 65524 },
	{ 16, 65525 },
	{ 15, 32707 },
	{ 16, 65526 },
	{ 16, 65527 },
	{ 16, 65528 },
	{ 16, 65529 },
	{ 16, 65530 },
	{ 16, 65531 },
	{ 16, 65532 },
	{ 16, 65533 },
	{ 16, 65534 }
};

typedef struct byte_buffer{
	unsigned char tmp_buffer_index;
	unsigned int byte_buffer_index;

	unsigned char tmp_buffer[8];
	unsigned char* byte_buffer;

	unsigned int byte_buffer_size;
}BYTE_BUFFER;

typedef struct output_stream{
	unsigned char* stream;
	unsigned int index;
}OUTPUT_STREAM;

void init_byte_buffer(BYTE_BUFFER* byte_buffer);
void put_bit_into_byte_buffer(BYTE_BUFFER* byte_buffer, unsigned char bit_value);
unsigned int get_byte_buffer_size(BYTE_BUFFER* byte_buffer);
void round_byte_buffer(BYTE_BUFFER* byte_buffer);
int get_luminance_ac_index(int zero_run_length, int category);
unsigned char get_size(short value);
unsigned int code_block(char* component_data, unsigned int component_data_index, BYTE_BUFFER* byte_buffer, unsigned short dc_table[160][2], unsigned short ac_table[160][2], unsigned short zrl[2], unsigned short eob[2]);
void form_jpeg(OUTPUT_STREAM* output_stream, BYTE_BUFFER* data);
#endif