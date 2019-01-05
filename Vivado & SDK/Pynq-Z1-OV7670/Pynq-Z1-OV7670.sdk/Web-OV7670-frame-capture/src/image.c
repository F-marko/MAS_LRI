#include "image.h"

RAW_YCbCr_IMAGE image;

void transformYUV422ImageToYUV() {
	u8* data = (u8*) DEST_ADDR;

	for (int i = 0; i < 480; ++i) {
		for (int j = 0; j < 640; ++j) {
			if(j % 2 == 0) {
				image.ycbcr[i][j].Y = data[i * 1280 + (j << 1) + 3];
				image.ycbcr[i][j].Cb = data[i * 1280 + (j << 1) + 2];
				image.ycbcr[i][j + 1].Cb = data[i * 1280 + (j << 1) + 2];
			} else {
				image.ycbcr[i][j].Y = data[i * 1280 + (j << 1) - 1];
				image.ycbcr[i][j].Cr = data[i * 1280 + (j << 1)];
				image.ycbcr[i][j - 1].Cr = data[i * 1280 + (j << 1)];
			}

		}
	}

	//usleep(500*1000); // wait 300 ms
}
