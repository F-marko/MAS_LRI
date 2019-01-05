/*
 * vdma_single_step.c
 *
 *  Created on: 19. pro 2018.
 *      Author: Luka
 */

#include "vdma_single_step.h"

XAxiVdma_Config *vdma_config;
XAxiVdma_DmaSetup vdma_w_setup;

// variables for endig loop while VDMA transfer is taking place
#define WAIT 4000
#define WAIT_SECONDS_COUNT ((XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ) / (WAIT))

// initialization flag for VDMA
u8 init_done = 0;

int vdma_ss_init(XAxiVdma *vdma, u32 width, u32 height, u16 deviceId, UINTPTR vdma_address, UINTPTR memory_addr)
{

	int status;
	u32 i;
	u32 addr;

	// look up for vdma config set in vivado gui
	vdma_config = NULL;
	vdma_config = XAxiVdma_LookupConfig(deviceId);
	if (vdma_config == NULL) {
		xil_printf("VDMA device not found\r\n");
		return XST_FAILURE;
	}
	// initialize vdma
	status = XAxiVdma_CfgInitialize(vdma, vdma_config, vdma_address);
	if (status != XST_SUCCESS) {
		xil_printf("VDMA config initialization failed\r\n");
		return XST_FAILURE;
	}

	// set the write channel in dma setup variable
	if (init_done != 1)
	{
		vdma_w_setup.EnableCircularBuf = 0;
		vdma_w_setup.EnableFrameCounter = 0;
		vdma_w_setup.EnableSync = 0;
		vdma_w_setup.FixedFrameStoreAddr = 0;
		vdma_w_setup.FrameDelay = 0;
		vdma_w_setup.GenLockRepeat = 0;
		vdma_w_setup.HoriSizeInput = width * (vdma_config->S2MmStreamWidth >> 3);
		vdma_w_setup.PointNum = 0;
		vdma_w_setup.Stride = vdma_w_setup.HoriSizeInput;
		vdma_w_setup.VertSizeInput = height;

		addr = memory_addr;
		for(i = 0; i < vdma->MaxNumFrames; i++)
		{
			vdma_w_setup.FrameStoreStartAddr[i] = addr;
			addr += (vdma_w_setup.HoriSizeInput * vdma_w_setup.VertSizeInput);
		}

		init_done = 1;
	}

	// restart vdma
	return vdma_ss_config(vdma);
}

int vdma_ss_config(XAxiVdma *vdma)
{

	int status;
	// turn off the frame counter until vdma transfer is "locked"
	vdma_w_setup.EnableFrameCounter = 0;
	// set up the write dma channel
	status = XAxiVdma_DmaConfig(vdma, XAXIVDMA_WRITE, &vdma_w_setup);
	if (status != XST_SUCCESS)
	{
		xil_printf("Write configuration failed %d\r\n", status);
		return XST_FAILURE;
	}


	// Set the buffer addresses for transfer in the DMA engine
	status = XAxiVdma_DmaSetBufferAddr(vdma, XAXIVDMA_WRITE, vdma_w_setup.FrameStoreStartAddr);
	if (status != XST_SUCCESS)
	{
		xil_printf("Write channel set buffer address failed %d\r\n", status);
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int vdma_ss_enable_frmcnt(XAxiVdma *vdma, u8 frmcnt)
{
	XAxiVdma_FrameCounter vdma_cnt;

	int status;
	// set the frame count
	vdma_cnt.ReadDelayTimerCount = 0;
	vdma_cnt.ReadFrameCount = 1;
	vdma_cnt.WriteDelayTimerCount = 0;
	vdma_cnt.WriteFrameCount = 1;
	status = XAxiVdma_SetFrameCounter(vdma, &vdma_cnt);
	if (status != XST_SUCCESS)
	{
		xil_printf("Set frame counter failed %d\r\n", status);
		return XST_FAILURE;
	}
	// enable the frame count
	XAxiVdma_StartFrmCntEnable(vdma, XAXIVDMA_WRITE);

	return XST_SUCCESS;
}

int vdma_ss_start(XAxiVdma *vdma)
{
	int status;
	u32 errors;
	// start the vdma transfer
	status = XAxiVdma_DmaStart(vdma, XAXIVDMA_WRITE);
	if (status != XST_SUCCESS)
	{
		xil_printf("VDMA start transfer failed");
		return XST_FAILURE;
	}

	// clear errors until the vdma transfer isn't "locked"
	do
	{
		//xil_printf("VDMA clear channel errors\r\n");
		errors = XAxiVdma_GetDmaChannelErrors(vdma, XAXIVDMA_WRITE);
		xil_printf("Errors: %X\r\n", errors);
		status = XAxiVdma_ClearDmaChannelErrors(vdma, XAXIVDMA_WRITE, errors);
		if (status != XST_SUCCESS)
		{
			xil_printf("VDMA clear error failed");
			return XST_FAILURE;
		}
	}while(errors != 0);

	return XST_SUCCESS;
}

// reset the vdma
int vdma_ss_reset(XAxiVdma *vdma)
{

	XAxiVdma_Reset(vdma, XAXIVDMA_WRITE);
	while(XAxiVdma_ResetNotDone(vdma, XAXIVDMA_WRITE));

	return vdma_ss_config(vdma);
}

// clear the frame count irq if set
void vdma_ss_clear_frmcnt_irq(XAxiVdma *vdma)
{
	u32 intr;

	intr = XAxiVdma_IntrGetPending(vdma, XAXIVDMA_WRITE);
	XAxiVdma_IntrClear(vdma, intr, XAXIVDMA_WRITE);
}

// test if vdma is done
u8 vdma_ss_done(XAxiVdma *vdma)
{
	u8 done;
	// halt flag
	done = (XAxiVdma_GetStatus(vdma, XAXIVDMA_WRITE) & 0x00000001);
	// frame count irq flag
	done &= (XAxiVdma_GetStatus(vdma, XAXIVDMA_WRITE) & 0x00001000) >> 12;

	return done;
}

// top level get frame
int vdma_ss_get_frame(XAxiVdma *vdma)
{
	int status, errors;
	u8 frame_count = 1;

	unsigned long i = 0;

	XAxiVdma_DmaRegisterDump(vdma, XAXIVDMA_WRITE);

	// start the transfer
	status = vdma_ss_start(vdma);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	XAxiVdma_DmaRegisterDump(vdma, XAXIVDMA_WRITE);
	// enable the frame counter
	status = vdma_ss_enable_frmcnt(vdma, frame_count);
	if (status != XST_SUCCESS)
	{
		return XST_FAILURE;
	}

	// clear the existing irqs
	vdma_ss_clear_frmcnt_irq(vdma);
	XAxiVdma_DmaRegisterDump(vdma, XAXIVDMA_WRITE);

	// wait until transfer is done or the time for transfer has run out
	while((vdma_ss_done(vdma) != 1) && (i < WAIT_SECONDS_COUNT))
	{
		// clear any new errors
		errors = XAxiVdma_GetDmaChannelErrors(vdma, XAXIVDMA_WRITE);
		xil_printf("Errors: %X\r\n", errors);
		status = XAxiVdma_ClearDmaChannelErrors(vdma, XAXIVDMA_WRITE, errors);
		if (status != XST_SUCCESS)
		{
			xil_printf("VDMA clear error failed");
			return XST_FAILURE;
		}
		XAxiVdma_DmaRegisterDump(vdma, XAXIVDMA_WRITE);
		i++;
	}

	// if the time has run out return error
	if (i >= WAIT_SECONDS_COUNT)
	{
		xil_printf("ERROR: vdma_ss_get_frame; time out\r\n");
		vdma_ss_reset(vdma);
		return -1;
	}

	return vdma_ss_reset(vdma);
}
