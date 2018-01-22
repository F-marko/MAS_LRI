#ifndef SRC_IMAGE_H_
#define SRC_IMAGE_H_
#include "xbasic_types.h"
// ====================================== DOHVAT S KAMERE =================================
#include "xgpio_l.h"
#include "image.h"

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

u8 Y[480][640];
u8 U[480][640];
u8 V[480][640];

void getImage();

#endif /* SRC_IMAGE_H_ */
