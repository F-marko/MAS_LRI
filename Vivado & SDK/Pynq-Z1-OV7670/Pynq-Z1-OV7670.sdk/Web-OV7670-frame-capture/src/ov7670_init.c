/*
 * Camera_init.c
 *
 *  Created on: 22. pro 2017.
 *      Author: Mateo
 */
#include "xparameters.h"
#include "sleep.h"
#include "xiicps.h"
#include "xil_printf.h"
#include "ov7670_init.h"

#define IIC_DEVICE_ID		XPAR_XIICPS_0_DEVICE_ID

XIicPs Iic;

int InitCamera()
{
	int Status, result;
	XIicPs_Config *I2C_Config;	/**< configuration information for the device */

	I2C_Config = XIicPs_LookupConfig(IIC_DEVICE_ID);
	if(I2C_Config == NULL){
		xil_printf("Error: XIicPs_LookupConfig()\n");
		return XST_FAILURE;
	}

	Status = XIicPs_CfgInitialize(&Iic, I2C_Config, I2C_Config->BaseAddress);
	if(Status != XST_SUCCESS){
		xil_printf("Error: XIicPs_CfgInitialize()\n");
		return XST_FAILURE;
	}

	Status = XIicPs_SelfTest(&Iic);
	if(Status != XST_SUCCESS){
		xil_printf("Error: XIicPs_SelfTest()\n");
		return XST_FAILURE;
	}

	XIicPs_SetSClk(&Iic, IIC_SCLK_RATE);
	xil_printf("I2C configuration done.\r\n");

	xil_printf("Soft Rest OV7670.\r\n");
	result = WriteReg(REG_COM7, COM7_RESET);
	if(result != XST_SUCCESS){
		xil_printf("Error: OV7670 RESET\r\n");
		return XST_FAILURE;
	}
	usleep(300*1000); // wait 300 ms

	return XST_SUCCESS;
}


int WriteReg(u8 reg, u8 value)
{
	int Status;
	u8 buff[2];

	buff[0] = reg;
	buff[1] = value;

	Status = XIicPs_MasterSendPolled(&Iic, buff, 2, OV7670ADDR_WRITE);

	if(Status != XST_SUCCESS){
		xil_printf("WriteReg:I2C Write Fail\r\n");
		return XST_FAILURE;
	}
	// Wait until bus is idle to start another transfer.
	while(XIicPs_BusIsBusy(&Iic)){
		asm("");
		/* NOP */
	}

	usleep(30*1000);	// wait 30ms

	return XST_SUCCESS;
}


int ReadReg(u8 reg)
{
	u8 buff[2];

	buff[0] = reg;
	XIicPs_MasterSendPolled(&Iic, buff, 1, OV7670ADDR_WRITE);
	while(XIicPs_BusIsBusy(&Iic)){
		/* NOP */
	}

	XIicPs_MasterRecvPolled(&Iic, buff, 1, OV7670ADDR_READ);
	while(XIicPs_BusIsBusy(&Iic)){
		asm("");
		/* NOP */
	}

	return buff[0];
}


void DumpReg(void)
{
	int i;

    xil_printf("AD : +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F") ;
    for (i=0; i < OV7670_REGMAX; i++) {
        int data ;
        data = ReadReg(i) ; // READ REG
        if ((i & 0x0F) == 0) {
            xil_printf("\n%02X : ", i) ;
        }
        xil_printf("%02X ",data) ;
    }
    xil_printf("\n") ;

}
//opca funkcija inicijalizacije
void InitRegs(struct regval_list *lista_postavki_reg) {

	int i = 0;
	while(lista_postavki_reg[i].reg_num != 0xff || lista_postavki_reg[i].reg_val != 0xff){
		u8 reg_adr = lista_postavki_reg[i].reg_num;
		u8 reg_val = lista_postavki_reg[i].reg_val;
		WriteReg(reg_adr, reg_val);
		//xil_printf("Pisem vrijednost %X u registar %X\r\n", reg_adr, reg_val);
		i++;
	}
}

//inicijalizacija default registara
void InitDefaultReg(struct regval_list *ov7670_default_regs) {
	InitRegs(ov7670_default_regs);
}

void InitRGB565(struct regval_list *ov7670_fmt_rgb565){
	InitRegs(ov7670_fmt_rgb565);
}
void InitYUV(struct regval_list *ov7670_fmt_yuv422){
	InitRegs(ov7670_fmt_yuv422);
}

void InitTestBar(struct regval_list *ov7670_test_bar){
	InitRegs(ov7670_test_bar);
}

void InitShiftingOnes(struct regval_list *ov7670_test_shift){
	InitRegs(ov7670_test_shift);
}

void InitImage(struct regval_list *ov7670_image)
{
	InitRegs(ov7670_image);
}






