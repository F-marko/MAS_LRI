/*
 * vdma_single_step.h
 *
 *  Created on: 19. pro 2018.
 *      Author: Luka
 */

#ifndef SRC_VDMA_SINGLE_STEP_H_
#define SRC_VDMA_SINGLE_STEP_H_

#include "xaxivdma.h"

#define DEST_ADDR 0x10000000
#define WIDTH 640
#define HEIGHT 480
#define FRAME_LINE ((WIDTH)*3)
#define FRAME_SIZE ((FRAME_LINE)*(HEIGHT))
#define FRAME_NUM 1

#define PRINT_WIDTH 64
#define PRINT_HEIGHT 64
#define PRINT_FRAME_LINE ((PRINT_WIDTH)*3)
#define PRINT_FRAME_SIZE ((PRINT_FRAME_LINE)*(PRINT_HEIGHT))

int vdma_ss_init(XAxiVdma *vdma, u32 width, u32 height, u16 deviceId, UINTPTR vdma_address, UINTPTR memory_addr);
int vdma_ss_config(XAxiVdma *vdma);
int vdma_ss_enable_frmcnt(XAxiVdma *vdma, u8 frmcnt);
int vdma_ss_start(XAxiVdma *vdma);
int vdma_ss_reset(XAxiVdma *vdma);
void vdma_ss_clear_frmcnt_irq(XAxiVdma *vdma);
u8 vdma_ss_done(XAxiVdma *vdma);
int vdma_ss_get_frame(XAxiVdma *vdma);


#endif /* SRC_VDMA_SINGLE_STEP_H_ */
