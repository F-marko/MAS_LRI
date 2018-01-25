/*
 * cameraSetup.h
 *
 *  Created on: 22. sij 2018.
 *      Author: Bicanic
 */

#ifndef SRC_CAMERASETUP_H_
#define SRC_CAMERASETUP_H_

// includes
#include "xiicps.h"
#include "cameraInit.h"

// defines
#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID
#define IIC_SLAVE_ADDR		0x21
#define IIC_SCLK_RATE		100000
#define TEST_BUFFER_SIZE	2

// global variables
XIicPs Iic; /**< Instance of the IIC Device */
u8 SendBuffer[TEST_BUFFER_SIZE]; /**< Buffer for Transmitting Data */

// function declarations
int IicPsMasterPolledExample(u16 DeviceId, struct regval_list *regConfig, int size);

#endif /* SRC_CAMERASETUP_H_ */
