/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

// ======================================== I2C TEST ======================================
#include "xiicps.h"
#include "cameraInit.h"
#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID
#define IIC_SLAVE_ADDR		0x21
#define IIC_SCLK_RATE		100000
#define TEST_BUFFER_SIZE	2

int IicPsMasterPolledExample(u16 DeviceId, struct regval_list *regConfig,
		int size);

XIicPs Iic; /**< Instance of the IIC Device */
u8 SendBuffer[TEST_BUFFER_SIZE]; /**< Buffer for Transmitting Data */
// ========================================================================================

// ====================================== DOHVAT S KAMERE =================================
#include "xgpio_l.h"

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
#include <stdio.h>

#include "xparameters.h"

#include "netif/xadapter.h"

#include "platform.h"
#include "platform_config.h"
#if defined (__arm__) || defined(__aarch64__)
#include "xil_printf.h"
#endif

#include "lwip/tcp.h"
#include "xil_cache.h"

#if LWIP_DHCP==1
#include "lwip/dhcp.h"
#endif

/* defined by each RAW mode application */
void print_app_header();
int start_application();
int transfer_data();
void tcp_fasttmr(void);
void tcp_slowtmr(void);

/* missing declaration in lwIP */
void lwip_init();

#if LWIP_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif

extern volatile int TcpFastTmrFlag;
extern volatile int TcpSlowTmrFlag;
static struct netif server_netif;
struct netif *echo_netif;

void print_ip(char *msg, struct ip_addr *ip) {
	print(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip), ip4_addr3(ip),
			ip4_addr4(ip));
}

void print_ip_settings(struct ip_addr *ip, struct ip_addr *mask,
		struct ip_addr *gw) {

	print_ip("Board IP: ", ip);
	print_ip("Netmask : ", mask);
	print_ip("Gateway : ", gw);
}

#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
int ProgramSi5324(void);
int ProgramSfpPhy(void);
#endif
#endif

#ifdef XPS_BOARD_ZCU102
#ifdef XPAR_XIICPS_0_DEVICE_ID
int IicPhyReset(void);
#endif
#endif

int main() {

	// ================================== I2C TEST ===================================
	int Status;

	xil_printf("IIC Master Polled Example Test \r\n");

	/*
	 * Run the Iic polled example in master mode, specify the Device
	 * ID that is specified in xparameters.h.
	 */
	Status = IicPsMasterPolledExample(IIC_DEVICE_ID, ov7670_default_regs,
			sizeof(ov7670_default_regs) / 2);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC Master Polled Example Test Failed 1\r\n");
		return XST_FAILURE;
	}
	Status = IicPsMasterPolledExample(IIC_DEVICE_ID, ov7670_fmt_yuv422,
			sizeof(ov7670_fmt_yuv422) / 2);
	if (Status != XST_SUCCESS) {
		xil_printf("IIC Master Polled Example Test Failed 2\r\n");
		return XST_FAILURE;
	}

	xil_printf("Successfully ran IIC Master Polled Example Test\r\n");

	for (u32 i = 0; i < DELAY; i++)
		;

	// ===============================================================================
	XGpio_WriteReg((GPIO_REG_BASEADDR), XGPIO_TRI_OFFSET, 0x7FF);
	XGpio_WriteReg((XPAR_GPIO_1_BASEADDR), XGPIO_TRI_OFFSET, 0x00);
	//XGpio_WriteReg((GPIO_REG_BASEADDR), XGPIO_CHAN_OFFSET + XGPIO_TRI_OFFSET, 0xFF);
	// ================================= DOHVAT S KAMERE ==================================
	u8 data[480 * 1280];
	u8 Y[480][640];
	u8 U[480][640];
	u8 V[480][640];
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
				printf("VSINC error [%d]\n", y);
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
					printf("HREF error [%d]\n", r);
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
	printf("OK !!!\n");
	/*
	 for(int i = 0; i < 100; i++)
	 {
	 if (i % 4 == 0)
	 xil_printf("\r\n");
	 xil_printf("%u ", data[i]);
	 }*/

	for(int i = 0; i < 480; ++i){
		for(int j = 0; j < 640; ++j){
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

	for (int i = 0; i < 100; ++i){
		xil_printf("Y=%d U=%d V=%d\r\n", Y[0][i], U[0][i], V[0][i]);
	}

	// =====================================================================================
	struct ip_addr ipaddr, netmask, gw;
	/* the mac address of the board. this should be unique per board */
	unsigned char mac_ethernet_address[] =
			{ 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };

	echo_netif = &server_netif;
#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
	ProgramSi5324();
	ProgramSfpPhy();
#endif
#endif

	/* Define this board specific macro in order perform PHY reset on ZCU102 */
#ifdef XPS_BOARD_ZCU102
	IicPhyReset();
#endif

	init_platform();

#if LWIP_DHCP==1
	ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;
#else
	/* initliaze IP addresses to be used */
	IP4_ADDR(&ipaddr, 192, 168, 1, 10);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
	IP4_ADDR(&gw, 192, 168, 1, 1);
#endif	
	print_app_header();

	lwip_init();

	/* Add network interface to the netif_list, and set it as default */
	if (!xemac_add(echo_netif, &ipaddr, &netmask, &gw, mac_ethernet_address,
	PLATFORM_EMAC_BASEADDR)) {
		xil_printf("Error adding N/W interface\n\r");
		return -1;
	}
	netif_set_default(echo_netif);

	/* now enable interrupts */
	platform_enable_interrupts();

	/* specify that the network if is up */
	netif_set_up(echo_netif);

#if (LWIP_DHCP==1)
	/* Create a new DHCP client for this interface.
	 * Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
	 * the predefined regular intervals after starting the client.
	 */
	dhcp_start(echo_netif);
	dhcp_timoutcntr = 24;

	while(((echo_netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0))
	xemacif_input(echo_netif);

	if (dhcp_timoutcntr <= 0) {
		if ((echo_netif->ip_addr.addr) == 0) {
			xil_printf("DHCP Timeout\r\n");
			xil_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(echo_netif->ip_addr), 192, 168, 1, 10);
			IP4_ADDR(&(echo_netif->netmask), 255, 255, 255, 0);
			IP4_ADDR(&(echo_netif->gw), 192, 168, 1, 1);
		}
	}

	ipaddr.addr = echo_netif->ip_addr.addr;
	gw.addr = echo_netif->gw.addr;
	netmask.addr = echo_netif->netmask.addr;
#endif

	print_ip_settings(&ipaddr, &netmask, &gw);

	/* start the application (web server, rxtest, txtest, etc..) */
	start_application();

	/* receive and process packets */
	while (1) {
		if (TcpFastTmrFlag) {
			tcp_fasttmr();
			TcpFastTmrFlag = 0;
		}
		if (TcpSlowTmrFlag) {
			tcp_slowtmr();
			TcpSlowTmrFlag = 0;
		}
		xemacif_input(echo_netif);
		transfer_data();
	}

	/* never reached */
	cleanup_platform();

	return 0;
}

// ===================================== I2C TEST ============================================================
int IicPsMasterPolledExample(u16 DeviceId, struct regval_list *regConfig,
		int size) {
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

// ==========================================================================================================
