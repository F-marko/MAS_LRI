#define _CRT_SECURE_NO_WARNINGS

#include "dct.h"
#include "huffman_coding.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAGIC_NUMBER_LENGTH		4

typedef struct ppm_header{
	char magic_number[MAGIC_NUMBER_LENGTH];
	int width;
	int height;
	int max_color_value;
}PPM_HEADER;

typedef struct pixel_rgb{
	short R;
	short G;
	short B;
}PIXEL_RGB;

typedef struct raw_rgb_image{
	PIXEL_RGB** rgb;
}RAW_RGB_IMAGE;

PPM_HEADER* get_ppm_header(char* filename){
	PPM_HEADER* ppm_header;
	FILE* file;

	file = fopen(filename, "r");
	if (file == NULL){
		printf("Can't open file %s.", filename);
		exit(1);
	}

	ppm_header = (PPM_HEADER*)malloc(sizeof(PPM_HEADER));
	fscanf(file, "%s", ppm_header->magic_number);
	fscanf(file, "%d", &(ppm_header->width));
	fscanf(file, "%d", &(ppm_header->height));
	fscanf(file, "%d", &(ppm_header->max_color_value));
	fclose(file);

	return ppm_header;
}

unsigned int get_ppm_header_size(PPM_HEADER* ppm_header){
	unsigned int offset = 0;
	char buffer[6];

	offset += strlen(ppm_header->magic_number);
	offset++;
	sprintf(buffer, "%d", ppm_header->width);
	offset += strlen(buffer);
	offset++;
	sprintf(buffer, "%d", ppm_header->height);
	offset += strlen(buffer);
	offset++;
	sprintf(buffer, "%d", ppm_header->max_color_value);
	offset += strlen(buffer);
	offset++;

	return offset;
}

void init_raw_rgb_image(RAW_RGB_IMAGE* raw_rgb_imege, PPM_HEADER* ppm_header){
	raw_rgb_imege->rgb = (PIXEL_RGB**)malloc(sizeof(PIXEL_RGB*)* ppm_header->height);
	for (int i = 0; i < ppm_header->height; i++){
		raw_rgb_imege->rgb[i] = (PIXEL_RGB*)malloc(sizeof(PIXEL_RGB)* ppm_header->width);
	}
}

void free_raw_rgb_image(RAW_RGB_IMAGE* raw_rgb_imege, PPM_HEADER* ppm_header){
	for (int i = 0; i < ppm_header->height; i++){
		free(raw_rgb_imege->rgb[i]);
	}
	free(raw_rgb_imege->rgb);
}

void read_raw_rgb_image(char* filename, RAW_RGB_IMAGE* raw_rgb_image, PPM_HEADER* ppm_header){
	FILE* file;
	unsigned char rgb_read_buffer[3];

	file = fopen(filename, "rb");
	fseek(file, get_ppm_header_size(ppm_header), SEEK_SET);

	for (int x = 0; x < ppm_header->height; x++){
		for (int y = 0; y < ppm_header->width; y++){
			fread(rgb_read_buffer, sizeof(char), 3, file);
			raw_rgb_image->rgb[x][y].R = rgb_read_buffer[0];
			raw_rgb_image->rgb[x][y].G = rgb_read_buffer[1];
			raw_rgb_image->rgb[x][y].B = rgb_read_buffer[2];
		}
	}

	fclose(file);
}

void rgb_to_ycbcr(RAW_RGB_IMAGE* raw_rgb_image, RAW_YCbCr_IMAGE* raw_ycbcr_image, PPM_HEADER* ppm_header){
	for (int x = 0; x < ppm_header->height; x++){
		for (int y = 0; y < ppm_header->width; y++){
			
			raw_ycbcr_image->ycbcr[x][y].Y = (short)(0.299 * raw_rgb_image->rgb[x][y].R + 0.587 * raw_rgb_image->rgb[x][y].G + 0.114 * raw_rgb_image->rgb[x][y].B);
			raw_ycbcr_image->ycbcr[x][y].Cb = (short)(-0.1687 * raw_rgb_image->rgb[x][y].R - 0.3313 * raw_rgb_image->rgb[x][y].G + 0.5 * raw_rgb_image->rgb[x][y].B + 128);
			raw_ycbcr_image->ycbcr[x][y].Cr = (short)(0.5 * raw_rgb_image->rgb[x][y].R - 0.4187 * raw_rgb_image->rgb[x][y].G - 0.0813 * raw_rgb_image->rgb[x][y].B + 128);
			
			/*
			raw_ycbcr_image->ycbcr[x][y].Y = (short) ((raw_rgb_image->rgb[x][y].R >> 2) + (raw_rgb_image->rgb[x][y].G >> 1) + (raw_rgb_image->rgb[x][y].B >> 3));
			raw_ycbcr_image->ycbcr[x][y].Cb = (short)(-(raw_rgb_image->rgb[x][y].R >> 3) - (raw_rgb_image->rgb[x][y].G >> 2) + (raw_rgb_image->rgb[x][y].B >> 1) + 128);
			raw_ycbcr_image->ycbcr[x][y].Cr = (short)((raw_rgb_image->rgb[x][y].R >> 1) - (raw_rgb_image->rgb[x][y].G >> 1) + 128);
			*/
		}
	}
}

OUTPUT_STREAM* write_out(RAW_YCbCr_IMAGE* raw_ycbcr_image, unsigned short width, unsigned short height){
	unsigned short n_x_blocks = height / 8;
	unsigned short n_y_blocks = width / 8;

	YCbCr_BLOCK ycbcr_block;

	unsigned char temp_buffer_index;
	char temp_buffer[64];

	// buffer koji sadr�i Y komponente slike zpisane u cik-cak redosljedu
	unsigned int y_buffer_index;
	char* y_buffer = (char*)malloc(height * width);
	// buffer koji sadr�i Cb komponente slike zpisane u cik-cak redosljedu
	unsigned int cb_buffer_index;
	char* cb_buffer = (char*)malloc(height * width);
	// buffer koji sadr�i Cr komponente slike zpisane u cik-cak redosljedu
	unsigned int cr_buffer_index;
	char* cr_buffer = (char*)malloc(height * width);

	unsigned short x;
	unsigned short y;

	BYTE_BUFFER binary_buffer;
	init_byte_buffer(&binary_buffer);

	y_buffer_index = 0;
	cb_buffer_index = 0;
	cr_buffer_index = 0;

	for (x = 0; x < n_x_blocks; x++){
		for (y = 0; y < n_y_blocks; y++){
			read_ycbcr_block(raw_ycbcr_image, x, y, &ycbcr_block);
			// spremi blok Y komponente u y_buffer u cik-cak redosljedu
			temp_buffer_index = 0;
			for (int u = 0; u < 8; u++){
				for (int v = 0; v < 8; v++){
					temp_buffer[temp_buffer_index] = (char)ycbcr_block.ycbcr[u][v].Y;
					++temp_buffer_index;
				}
			}
			for (int n = 0; n < 64; n++){
				y_buffer[y_buffer_index] = temp_buffer[zig_zag[n]];
				++y_buffer_index;
			}
			// spremi blok Cb komponente u cb_buffer u cik-cak redosljedu
			temp_buffer_index = 0;
			for (int u = 0; u < 8; u++){
				for (int v = 0; v < 8; v++){
					temp_buffer[temp_buffer_index] = (char)ycbcr_block.ycbcr[u][v].Cb;
					++temp_buffer_index;
				}
			}
			for (int n = 0; n < 64; n++){
				cb_buffer[cb_buffer_index] = temp_buffer[zig_zag[n]];
				++cb_buffer_index;
			}
			// spremi blok Cr komponente u cr_buffer u cik-cak redosljedu
			temp_buffer_index = 0;
			for (int u = 0; u < 8; u++){
				for (int v = 0; v < 8; v++){
					temp_buffer[temp_buffer_index] = (char)ycbcr_block.ycbcr[u][v].Cr;
					++temp_buffer_index;
				}
			}
			for (int n = 0; n < 64; n++){
				cr_buffer[cr_buffer_index] = temp_buffer[zig_zag[n]];
				++cr_buffer_index;
			}
		}
	}

	unsigned int block_counter = 0;
	unsigned int number_of_blocks = (height * width * 3) / 64;

	y_buffer_index = 0;
	cb_buffer_index = 0;
	cr_buffer_index = 0;

	// sve tri komponente su u jednom scan bloku pa ih je portebno kodirati isprepeleteno po blokovima,
	// tj. Y1 Cb1 Cr1 Y2 Cb2 Cr2 ...
	while (block_counter < number_of_blocks){
		// code one block of y component
		y_buffer_index = code_block(y_buffer, y_buffer_index, &binary_buffer, dc_luminance, ac_luminance, ac_zrl_lum, ac_eob_lum);
		// code one block of cb component
		cb_buffer_index = code_block(cb_buffer, cb_buffer_index, &binary_buffer, dc_chrominance, ac_chrominance, ac_zrl_chrom, ac_eob_chrom);
		// code one block of cr component
		cr_buffer_index = code_block(cr_buffer, cr_buffer_index, &binary_buffer, dc_chrominance, ac_chrominance, ac_zrl_chrom, ac_eob_chrom);

		block_counter = block_counter + 3;
	}

	OUTPUT_STREAM* output_stream;
	output_stream = (OUTPUT_STREAM*)malloc(sizeof(OUTPUT_STREAM));
	// upisuje u output_stream kodirane podatke i sve dodatne podatke koje zahtjeva jpeg format
	form_jpeg(output_stream, &binary_buffer);

	/*
	FILE* output_file = fopen("lenna.jpg", "wb");
	fwrite(output_stream.stream, 1, output_stream.index, output_file);
	fclose(output_file);
	*/

	return output_stream;
}

int main(int argc, char** argv){
	PPM_HEADER* ppm_header;
	FILE* test;
	RAW_RGB_IMAGE raw_rgb_image;
	RAW_YCbCr_IMAGE raw_ycbcr_image;
	unsigned int ppm_header_size;

	if (argc != 2){
		return 1;
	}
	test = fopen(argv[1], "r");
	if (test == NULL){
		return 1;
	}
	fclose(test);

	ppm_header = get_ppm_header(argv[1]);
	ppm_header_size = get_ppm_header_size(ppm_header);

	init_raw_rgb_image(&raw_rgb_image, ppm_header);
	printf("Reading image.\n");
	read_raw_rgb_image(argv[1], &raw_rgb_image, ppm_header);
	init_raw_ycbcr_image(&raw_ycbcr_image, ppm_header->width, ppm_header->height);
	printf("Converting from RGB to YCbCr.\n");
	rgb_to_ycbcr(&raw_rgb_image, &raw_ycbcr_image, ppm_header);

	free_raw_rgb_image(&raw_rgb_image, ppm_header);
	printf("Performing DCT.\n");
	dct(&raw_ycbcr_image, ppm_header->width, ppm_header->height);
	printf("Writing out.\n");
	OUTPUT_STREAM* os;
	os = write_out(&raw_ycbcr_image, ppm_header->width, ppm_header->height);
	FILE* output_file = fopen("lenna.jpg", "wb");
	fwrite(os->stream, 1, os->index, output_file);
	fclose(output_file);


	free_raw_ycbcr_image(&raw_ycbcr_image, ppm_header->width, ppm_header->height);
	return 0;
}