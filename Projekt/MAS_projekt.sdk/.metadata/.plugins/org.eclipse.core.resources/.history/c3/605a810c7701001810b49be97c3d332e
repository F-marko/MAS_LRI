#include "image.h"

void getImageFromBram() {
	unsigned char* address = (unsigned char*) XPAR_BRAM_0_BASEADDR;

	for (int i = 0; i < 480; i++) {
		for (int j = 0; j < 640; j += 2) {
			image.ycbcr[i][j].Y = *(address + 3); // LITTLE ENDIAN - Y1 je na lokacija + 3
			//xil_printf("Y1 = %X\n", *(address + 3));

			image.ycbcr[i][j].Cb = *(address + 2); // LITTLE ENDIAN - U je na lokacija + 2
			image.ycbcr[i][j + 1].Cb = *(address + 2);
			//xil_printf("U = %X\n", *(address + 2));

			image.ycbcr[i][j + 1].Y = *(address + 1); // LITTLE ENDIAN - V je na lokacija + 1
			//xil_printf("Y2 = %X\n", *(address + 1));

			image.ycbcr[i][j].Cr = *(address); // LITTLE ENDIAN - Y2 je na lokacija + 0
			image.ycbcr[i][j + 1].Cr = *(address);
			//xil_printf("V = %X\n", *(address));
		}
	}
}

void getImage() {

	u8 led = 3;
	int z = 0;
	u16 tt, tt1, r, y;
	ll: z = 0;
	led = ~led;
	WRITE_LED(led);
	while (CAMERA_isVSYNdown(READ_CAM))
		;
	while (CAMERA_isVSYNup(READ_CAM))
		;

	for (y = 0; y < 480; y++) {  //288
		//Wait line to start
		do {
			tt = READ_CAM;
			if (CAMERA_isVSYNup(tt)) {
				xil_printf("VSINC error [%d]\n", y);
				goto ll;
			}
		} while (CAMERA_isHREFdown(tt));
		//Y[z] =(u8)(tt);
		//z++;
		for (r = 0; r < 1280; r++) { //352
			do {
				tt = READ_CAM;
			} while (CAMERA_isPCLKdown(tt)); //while(CAMERA_isPCLKup(tt));

			//---------
			do {
				tt1 = READ_CAM;
				if (CAMERA_isHREFdown(tt1) && r != 1279) {
					xil_printf("HREF error [%d]\n", r);
					goto ll;
				}
			} while (CAMERA_isPCLKup(tt1));

			//Write data
			//HMY[z] =(Xuint8)(tt>>3);
			data[z] = (u8) (tt);
			z++;
		}
		while (CAMERA_isHREFup(READ_CAM))
			;

	}
	//xil_printf("%d",z);
	xil_printf("OK !!!\n");
	/*
	 for(int i = 0; i < 100; i++)
	 {
	 if (i % 4 == 0)
	 xil_printf("\r\n");
	 xil_printf("%u ", data[i]);
	 }*/

	for (int i = 0; i < 480; ++i) {
		for (int j = 0; j < 640; ++j) {
			image.ycbcr[i][j].Y = data[i * 1280 + (j << 1)];
			if (j % 2 == 0) {
				image.ycbcr[i][j].Cb = data[i * 1280 + (j << 1) + 1];
				image.ycbcr[i][j + 1].Cb = data[i * 1280 + (j << 1) + 1];
			} else {
				image.ycbcr[i][j].Cr = data[i * 1280 + (j << 1) + 1];
				image.ycbcr[i][j - 1].Cr = data[i * 1280 + (j << 1) + 1];
			}
		}
	}

	xil_printf("Prosao punjenje strukture\r\n");
}

void YUV422ToYUV() {
	for (int i = 0; i < 480; ++i) {
		for (int j = 0; j < 640; ++j) {
			YUVPacked[i * 640 * 3 + j * 3] = data[i * 1280 + (j << 1)];
			if (j % 2 == 0) {
				YUVPacked[i * 640 * 3 + j * 3 + 1] = data[i * 1280 + (j << 1)
						+ 1];
				YUVPacked[i * 640 * 3 + j * 3 + 4] = data[i * 1280 + (j << 1)
						+ 1];
			} else {
				YUVPacked[i * 640 * 3 + j * 3 - 1] = data[i * 1280 + (j << 1)
						+ 1];
				YUVPacked[i * 640 * 3 + j * 3 + 2] = data[i * 1280 + (j << 1)
						+ 1];
			}
		}
	}
}
