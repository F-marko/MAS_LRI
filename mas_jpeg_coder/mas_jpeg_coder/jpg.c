#include "jpg.h"

OUTPUT_STREAM* make_jpg_image(RAW_YCbCr_IMAGE* raw_ycbcr_image, unsigned short width, unsigned short height){
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

	dct(raw_ycbcr_image, width, height);

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

	free(y_buffer);
	free(cb_buffer);
	free(cr_buffer);

	OUTPUT_STREAM* output_stream;
	output_stream = (OUTPUT_STREAM*)malloc(sizeof(OUTPUT_STREAM));
	// upisuje u output_stream kodirane podatke i sve dodatne podatke koje zahtjeva jpeg format
	form_jpeg(output_stream, &binary_buffer);

	free_byte_buffer(&binary_buffer);

	return output_stream;
}
