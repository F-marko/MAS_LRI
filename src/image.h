#ifndef SRC_IMAGE_H_
#define SRC_IMAGE_H_
#include "xbasic_types.h"
#include "jpg.h"


// ====================================== DOHVAT S KAMERE =================================
#include "xgpio_l.h"
#include "image.h"

#define XOR_MASK 0xCA

#define IMAGE_HEIGHT 480
#define IMAGE_WIDTH 640

#define GPIO_REG_BASEADDR	XPAR_GPIO_0_BASEADDR

#define VSYN_MASK 0x100
#define HREF_MASK 0x200
#define PCLK_MASK 0x400

#define CAMERA_isVSYNup(X)     ((X) & VSYN_MASK)
#define CAMERA_isVSYNdown(X)  (!((X) & VSYN_MASK))

#define CAMERA_isHREFdown(X) (!((X) & HREF_MASK))
#define CAMERA_isHREFup(X)     ((X) & HREF_MASK)

#define CAMERA_isPCLKdown(X) (!((X) & PCLK_MASK))
#define CAMERA_isPCLKup(X)     ((X) & PCLK_MASK)

#define READ_CAM			 XGpio_ReadReg(GPIO_REG_BASEADDR, XGPIO_DATA_OFFSET)
// ========================================================================================

// ======================================== UTILITY MACROS ================================
#define WRITE_LED(X) (XGpio_WriteReg((XPAR_GPIO_1_BASEADDR), XGPIO_DATA_OFFSET, (X)))
#define DELAY 100000000
// ========================================================================================

RAW_YCbCr_IMAGE image;
u8 data[480 * 1280];
u8 YUVPacked[640 * 480 * 3];
OUTPUT_STREAM* os;
/*u8 Y[480][640];
u8 U[480][640];
u8 V[480][640];*/

void getImage();

void getImageFromBram();

void YUV422ToYUV();

#endif /* SRC_IMAGE_H_ */
