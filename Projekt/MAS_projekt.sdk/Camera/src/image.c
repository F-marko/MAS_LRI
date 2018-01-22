#include "image.h"

void getImage(){

	u8 data[480 * 1280];
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
				Y[i][j] = data[i * 1280 + (j << 1)];
				if (j % 2 == 0) {
					U[i][j] = data[i * 1280 + (j << 1) + 1];
					U[i][j + 1] = data[i * 1280 + (j << 1) + 1];
				} else {
					V[i][j] = data[i * 1280 + (j << 1) + 1];
					V[i][j - 1] = data[i * 1280 + (j << 1) + 1];
				}
			}
		}
}
