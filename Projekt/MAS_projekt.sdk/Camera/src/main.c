
// ========================== NASI INCLUDEOVI ============================================

#include "cameraSetup.h" // setting up the camera via the I2C interface
#include "image.h" // grabbing the picture from the camera

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

	init_platform();
	xil_printf("Camera initialization started\r\n");

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
	Status = IicPsMasterPolledExample(IIC_DEVICE_ID,
			ov7670_fmt_yuv422,
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


	init_raw_ycbcr_image(&image, IMAGE_WIDTH, IMAGE_HEIGHT);
	getImage();
	os = make_jpg_image(&image, IMAGE_WIDTH, IMAGE_HEIGHT);


	/*for (int i = 0; i < 100; ++i) {
		xil_printf("Y=%d U=%d V=%d\r\n", Y[0][i], U[0][i], V[0][i]);
	}*/

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

	while (((echo_netif->ip_addr.addr) == 0) && (dhcp_timoutcntr > 0))
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
