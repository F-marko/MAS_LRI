#include "cameraSetup.h"


int IicPsMasterPolledExample(u16 DeviceId, struct regval_list *regConfig, int size) {
	int Status;
	XIicPs_Config *Config;
	int regIndex;

	/*
	 * Initialize the IIC driver so that it's ready to use
	 * Look up the configuration in the config table,
	 * then initialize it.
	 */
	Config = XIicPs_LookupConfig(DeviceId);
	if (NULL == Config) {
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(&Iic, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Perform a self-test to ensure that the hardware was built correctly.
	 */
	Status = XIicPs_SelfTest(&Iic);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);

	/*
	 * Initialize the send buffer bytes with a pattern to send and the
	 * the receive buffer bytes to zero to allow the receive data to be
	 * verified.
	 */

	for (regIndex = 0; regIndex < size; ++regIndex) {

		//SendBuffer[0] = ov7670_test_shift[regIndex].reg_num;
		//SendBuffer[1] = ov7670_test_shift[regIndex].value;

		/*
		 * Send the buffer using the IIC and ignore the number of bytes sent
		 * as the return value since we are using it in interrupt mode.
		 */
		Status = XIicPs_MasterSendPolled(&Iic, (u8 *) &regConfig[regIndex],
				sizeof(*regConfig), IIC_SLAVE_ADDR);
		if (Status != XST_SUCCESS) {
			return XST_FAILURE;
		}

		/*
		 * Wait until bus is idle to start another transfer.
		 */
		while (XIicPs_BusIsBusy(&Iic)) {
			/* NOP */
		}
	}

	return XST_SUCCESS;
}
