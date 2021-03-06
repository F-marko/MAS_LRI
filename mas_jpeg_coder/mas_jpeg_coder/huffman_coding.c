#include "huffman_coding.h"

void init_byte_buffer(BYTE_BUFFER* byte_buffer){
	byte_buffer->tmp_buffer_index = 0;

	byte_buffer->byte_buffer_size = 10000;
	byte_buffer->byte_buffer_index = 0;
	byte_buffer->byte_buffer = (unsigned char*)malloc(byte_buffer->byte_buffer_size);
}

void free_byte_buffer(BYTE_BUFFER* byte_buffer){
	free(byte_buffer->byte_buffer);
}

void put_bit_into_byte_buffer(BYTE_BUFFER* byte_buffer, unsigned char bit_value){
	unsigned char mask;
	unsigned char j, i;

	byte_buffer->tmp_buffer[byte_buffer->tmp_buffer_index] = bit_value;
	byte_buffer->tmp_buffer_index = byte_buffer->tmp_buffer_index + 1;

	if (byte_buffer->tmp_buffer_index == 8){
		j = 7;
		mask = 0;
		for (i = 0; i < 8; i++){
			if (byte_buffer->tmp_buffer[i] == 1){
				mask |= 1 << j;
			}
			else if (byte_buffer->tmp_buffer[i] == 0){
				mask |= 0 << j;
			}
			--j;
		}
		byte_buffer->byte_buffer[byte_buffer->byte_buffer_index] = mask;
		byte_buffer->byte_buffer_index = byte_buffer->byte_buffer_index + 1;

		if (byte_buffer->byte_buffer_index == byte_buffer->byte_buffer_size){
			byte_buffer->byte_buffer_size = byte_buffer->byte_buffer_size + 10000;
			byte_buffer->byte_buffer = realloc(byte_buffer->byte_buffer, byte_buffer->byte_buffer_size);
		}

		if (mask == 0xff){
			byte_buffer->byte_buffer[byte_buffer->byte_buffer_index] = 0;
			byte_buffer->byte_buffer_index = byte_buffer->byte_buffer_index + 1;
		}

		byte_buffer->tmp_buffer_index = 0;
	}
}

unsigned int get_byte_buffer_size(BYTE_BUFFER* byte_buffer){
	return byte_buffer->byte_buffer_index;
}

void round_byte_buffer(BYTE_BUFFER* byte_buffer){
	for (unsigned char i = byte_buffer->tmp_buffer_index; i < 8; i++){
		put_bit_into_byte_buffer(byte_buffer, 1);
	}
}

int get_luminance_ac_index(int zero_run_length, int category){
	return zero_run_length * 10 + category - 1;
}

unsigned char get_size(short value){
	unsigned char size;

	for (unsigned char i = 0; i < 12; i++){
		if (abs(value) < pow(2, i)){
			size = i;
			break;
		}
	}

	return size;
}

unsigned int code_block(char* component_data, unsigned int component_data_index, BYTE_BUFFER* byte_buffer, unsigned short dc_table[160][2], unsigned short ac_table[160][2], unsigned short zrl[2], unsigned short eob[2]){
	short dc_diff;
	short ac_cat;
	short ac_val;
	unsigned char dc_cat;

	unsigned char zero_run_length = 0;

	unsigned short mask;

	unsigned int data_index = component_data_index;

	int ac_table_index;

	unsigned char eob_flag;
	unsigned int tmp;

	while (1){
		// ako je DC koeficijent
		if (data_index % 64 == 0){
			if (data_index == 0){
				dc_diff = component_data[data_index];
			}
			else{
				dc_diff = component_data[data_index] - component_data[data_index - 64];
			}
			// odredi kategoriju DC koeficijenta
			dc_cat = get_size(dc_diff);
			if (dc_diff < 0){
				dc_diff = dc_diff - 1;
			}
			for (int n = dc_table[dc_cat][0] - 1; n >= 0; n--){
				mask = 1 << n;
				if (mask & dc_table[dc_cat][1]){
					put_bit_into_byte_buffer(byte_buffer, 1);
				}
				else{
					put_bit_into_byte_buffer(byte_buffer, 0);
				}
			}
			for (int n = dc_cat - 1; n >= 0; n--){
				mask = 1 << n;
				if (mask & dc_diff){
					put_bit_into_byte_buffer(byte_buffer, 1);
				}
				else{
					put_bit_into_byte_buffer(byte_buffer, 0);
				}
			}
			++data_index;
		}
		// ako je AC koeficijent
		else{
			if (component_data[data_index] == 0){
				++zero_run_length;

				if (zero_run_length == 16){
					eob_flag = 1;
					for (tmp = data_index; tmp % 64 != 0; tmp++){
						if (component_data[tmp] != 0){
							eob_flag = 0;
							break;
						}
					}
					if (eob_flag == 1){
						for (int n = eob[0] - 1; n >= 0; n--){
							mask = 1 << n;
							if (mask & eob[1]){
								put_bit_into_byte_buffer(byte_buffer, 1);
							}
							else{
								put_bit_into_byte_buffer(byte_buffer, 0);
							}
						}
						data_index = tmp;
						break;
					}
					else{
						for (int n = zrl[0] - 1; n >= 0; n--){
							mask = 1 << n;
							if (mask & zrl[1]){
								put_bit_into_byte_buffer(byte_buffer, 1);
							}
							else{
								put_bit_into_byte_buffer(byte_buffer, 0);
							}
						}
						++data_index;
					}
					zero_run_length = 0;
				}
				else{
					++data_index;
				}
			}
			else{
				ac_val = component_data[data_index];
				// odredi kategoriju AC koeficijenta
				ac_cat = get_size(ac_val);
				if (ac_val < 0){
					ac_val = ac_val - 1;
				}
				ac_table_index = get_luminance_ac_index(zero_run_length, ac_cat);

				for (int n = ac_table[ac_table_index][0] - 1; n >= 0; n--){
					mask = 1 << n;
					if (mask & ac_table[ac_table_index][1]){
						put_bit_into_byte_buffer(byte_buffer, 1);
					}
					else{
						put_bit_into_byte_buffer(byte_buffer, 0);
					}
				}

				for (int n = ac_cat - 1; n >= 0; n--){
					mask = 1 << n;
					if (mask & ac_val){
						put_bit_into_byte_buffer(byte_buffer, 1);
					}
					else{
						put_bit_into_byte_buffer(byte_buffer, 0);
					}
				}
				zero_run_length = 0;

				++data_index;
			}
		}
	}

	return data_index;
}

void write_data(OUTPUT_STREAM* output_stream, unsigned char* marker, unsigned int size){
	unsigned int i;
	
	for (i = 0; i < size; i++){
		output_stream->stream[output_stream->index] = marker[i];
		output_stream->index = output_stream->index + 1;
	}
}

void form_jpeg(OUTPUT_STREAM* output_stream, BYTE_BUFFER* data){
	unsigned int data_size;
	unsigned int output_stream_size;

	round_byte_buffer(data);
	data_size = get_byte_buffer_size(data);

	output_stream_size = jpeg_format_data_size + data_size;

	output_stream->index = 0;
	output_stream->stream = (unsigned char*)malloc(output_stream_size);

	// marker SOI (pocetak slike)
	write_data(output_stream, soi, 2);
	// marker EXT (specificira jiff norma)
	write_data(output_stream, ext, 2);
	// EXT zaglavlje
	write_data(output_stream, ext_header, 16);
	// marker DQT (marker za kvantizacijsku taablicu)
	write_data(output_stream, dqt, 2);
	// DQT podaci, za Y komponentu
	write_data(output_stream, dqt_lum_ctx, 67);
	// marker DQT (marker za kvantizacijsku taablicu)
	write_data(output_stream, dqt, 2);
	// DQT podaci, za Cb i Cr komponente
	write_data(output_stream, dqt_chr_ctx, 67);
	// marker SOF (pocetak okvira)
	write_data(output_stream, sof, 2);
	// SOF zaglavlje
	write_data(output_stream, frame_header, 17);
	
	// marker DHT (pocetak definicije huffmanove tablice)
	write_data(output_stream, dht, 2);
	// DC huffmanova tablica za Y komponentu
	write_data(output_stream, huffman_table_dc_luminance, 31);
	// marker DHT (pocetak definicije huffmanove tablice)
	write_data(output_stream, dht, 2);
	// AC huffmanova tablica za Y komponentu
	write_data(output_stream, huffman_table_ac_luminance, 181);
	// marker DHT (pocetak definicije huffmanove tablice)
	write_data(output_stream, dht, 2);
	// DC huffmanova tablica za Cb i Cr komponente
	write_data(output_stream, huffman_table_dc_chrominance, 31);
	// marker DHT (pocetak definicije huffmanove tablice)
	write_data(output_stream, dht, 2);
	// AC huffmanova tablica za Cb i Cr komponente
	write_data(output_stream, huffman_table_ac_chrominance, 181);

	// marker SOS (pocetak scana)
	write_data(output_stream, sos, 2);
	// SOS zaglavlje
	write_data(output_stream, scan_header, 12);

	// podaci
	write_data(output_stream, data->byte_buffer, data_size);

	// marker EOI (kraj slike)
	write_data(output_stream, eoi, 2);
}