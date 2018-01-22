#include "dct.h"

void read_ycbcr_block(RAW_YCbCr_IMAGE* raw_ycbcr_image, int A, int B, YCbCr_BLOCK* ycbcr_block){
	int start_x = A * 8;
	int start_y = B * 8;

	int X = 0;
	int Y = 0;

	int x;
	int y;

	for (x = start_x; x < start_x + 8; x++){
		Y = 0;
		for (y = start_y; y < start_y + 8; y++){
			ycbcr_block->ycbcr[X][Y].Y = raw_ycbcr_image->ycbcr[x][y].Y;
			ycbcr_block->ycbcr[X][Y].Cb = raw_ycbcr_image->ycbcr[x][y].Cb;
			ycbcr_block->ycbcr[X][Y].Cr = raw_ycbcr_image->ycbcr[x][y].Cr;
			Y++;
		}
		X++;
	}
}

void write_ycbcr_block(RAW_YCbCr_IMAGE* raw_ycbcr_image, int A, int B, YCbCr_BLOCK* ycbcr_block){
	int start_x = A * 8;
	int start_y = B * 8;

	int X = 0;
	int Y = 0;

	int x;
	int y;

	for (x = start_x; x < start_x + 8; x++){
		Y = 0;
		for (y = start_y; y < start_y + 8; y++){
			raw_ycbcr_image->ycbcr[x][y].Y = ycbcr_block->ycbcr[X][Y].Y;
			raw_ycbcr_image->ycbcr[x][y].Cb = ycbcr_block->ycbcr[X][Y].Cb;
			raw_ycbcr_image->ycbcr[x][y].Cr = ycbcr_block->ycbcr[X][Y].Cr;
			Y++;
		}
		X++;
	}
}

void shift_ycbcr_block(YCbCr_BLOCK* ycbcr_block){
	for (int x = 0; x < 8; x++){
		for (int y = 0; y < 8; y++){
			ycbcr_block->ycbcr[x][y].Y = ycbcr_block->ycbcr[x][y].Y - 128;
			ycbcr_block->ycbcr[x][y].Cb = ycbcr_block->ycbcr[x][y].Cb - 128;
			ycbcr_block->ycbcr[x][y].Cr = ycbcr_block->ycbcr[x][y].Cr - 128;
		}
	}
}

void block_dct(YCbCr_BLOCK* ycbcr_block, YCbCr_BLOCK* result){
	float sum_y;
	float sum_cb;
	float sum_cr;
	float Cu;
	float Cv;
	
	int u;
	int v;

	int i;
	int j;

	float cos_value;

	for (u = 0; u < 8; u++){
		for (v = 0; v < 8; v++){

			if (u >= 4 || v >= 4){
				result->ycbcr[u][v].Y = 0;
				result->ycbcr[u][v].Cb = 0;
				result->ycbcr[u][v].Cr = 0;
			}
			else{
				sum_y = 0;
				sum_cb = 0;
				sum_cr = 0;

				if (u == 0){
					//Cu = (float)(1.0 / (float)sqrt(2.0));
					Cu = (float)0.7071067812;
				}
				else{
					Cu = 1.0;
				}

				if (v == 0){
					//Cv = (float)(1.0 / (float)sqrt(2.0));
					Cv = (float)0.7071067812;;
				}
				else{
					Cv = 1.0;
				}

				for (i = 0; i < 8; i++){
					for (j = 0; j < 8; j++){
						cos_value = cos_lookup_table[u][i] * cos_lookup_table[v][j];
						//cos_value = (float)(cos(((2 * i + 1) * u * PI) / 16) * cos(((2 * j + 1) * v * PI) / 16));
						sum_y += ycbcr_block->ycbcr[i][j].Y * cos_value;
						sum_cb += ycbcr_block->ycbcr[i][j].Cb * cos_value;
						sum_cr += ycbcr_block->ycbcr[i][j].Cr * cos_value;
					}
				}

				result->ycbcr[u][v].Y = (short)(0.25 * Cu * Cv * sum_y);
				result->ycbcr[u][v].Cb = (short)(0.25 * Cu * Cv * sum_cb);
				result->ycbcr[u][v].Cr = (short)(0.25 * Cu * Cv * sum_cr);
			}
		}
	}
}

void quantize_ycbcr_block(YCbCr_BLOCK* ycbcr_block){
	float y, yk;
	float cb, cbk;
	float cr, crk;

	for (int u = 0; u < 8; u++){
		for (int v = 0; v < 8; v++){
			y = (float)ycbcr_block->ycbcr[u][v].Y;
			cb = (float)ycbcr_block->ycbcr[u][v].Cb;
			cr = (float)ycbcr_block->ycbcr[u][v].Cr;

			yk = (float)K1[u][v];
			cbk = (float)K2[u][v];
			crk = (float)K2[u][v];

			ycbcr_block->ycbcr[u][v].Y = (short)round(y / yk);
			ycbcr_block->ycbcr[u][v].Cb = (short)round(cb / cbk);
			ycbcr_block->ycbcr[u][v].Cr = (short)round(cr / crk);
		}
	}
}

void dct(RAW_YCbCr_IMAGE* raw_ycbcr_image, unsigned short width, unsigned short height){
	int n_x_blocks = height / 8;
	int n_y_blocks = width / 8;

	YCbCr_BLOCK ycbcr_block;
	YCbCr_BLOCK result_block;

	int n_of_pixels = height * width;

	int x;
	int y;

	for (x = 0; x < n_x_blocks; x++){
		for (y = 0; y < n_y_blocks; y++){
			read_ycbcr_block(raw_ycbcr_image, x, y, &ycbcr_block);

			shift_ycbcr_block(&ycbcr_block);

			block_dct(&ycbcr_block, &result_block);

			quantize_ycbcr_block(&result_block);

			write_ycbcr_block(raw_ycbcr_image, x, y, &result_block);
		}
	}
}

void init_raw_ycbcr_image(RAW_YCbCr_IMAGE* raw_ycbcr_image, unsigned short width, unsigned short height){
	raw_ycbcr_image->ycbcr = (PIXEL_YCbCr**)malloc(sizeof(PIXEL_YCbCr*)* height);
	for (int i = 0; i < height; i++){
		raw_ycbcr_image->ycbcr[i] = (PIXEL_YCbCr*)malloc(sizeof(PIXEL_YCbCr)* width);
	}
}

void free_raw_ycbcr_image(RAW_YCbCr_IMAGE* raw_ycbcr_image, unsigned short width, unsigned short height){
	for (int i = 0; i < height; i++){
		free(raw_ycbcr_image->ycbcr[i]);
	}
	free(raw_ycbcr_image->ycbcr);
}