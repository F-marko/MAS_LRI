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

#include <stdio.h>
#include <string.h>
#include "lwip/err.h"
#include "lwip/tcp.h"
#if defined (__arm__) || defined (__aarch64__)
#include "xil_printf.h"
#endif

#include "server.h"
#include "image.h"

#define BUFFER_SIZE 1024

// structure for sending data
typedef struct buffer_struct {
	uint8_t buf[BUFFER_SIZE];
	uint32_t offset;
} lwip_buffer;

lwip_buffer buffer; // buffer used for sending TCP data
int8_t current_mode = -1; // Current camera mode: testbar (0), image (1) or shifting ones (2)
OUTPUT_STREAM* os; // JPEG output stream to send



static void fill_buffer(uint8_t buffer[], uint8_t data[], uint32_t offset, int32_t size) {
	for (int32_t i = 0; i < size; ++i) {
		buffer[i] = data[offset + i];
	}
}


err_t sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
	int32_t sizeToSend;

	// done with sending
	if (buffer.offset >= os->length) {
		tcp_close(tpcb);
		buffer.offset = 0;
		free(os->stream);
		os->length = 0;
		xil_printf("Done sending JPEG\n");
		return ERR_OK;
	}

	if (buffer.offset >= os->length - BUFFER_SIZE) { // size left to send is less than whole buffer
		sizeToSend = os->length - buffer.offset;
	} else {
		sizeToSend = BUFFER_SIZE; // sending whole buffer
	}

	fill_buffer(buffer.buf, os->stream, buffer.offset, sizeToSend);

	tcp_write(tpcb, (void*) buffer.buf, sizeToSend, 0);

	buffer.offset += sizeToSend;

	return ERR_OK;
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	int sizeToSend;
	uint8_t req;

	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);

	req = *(uint8_t *) p->payload;

	if((req != 0) && (req != 1) && (req != 2)) {
		xil_printf("Bad request.\r\n");
		tcp_close(tpcb);
		return ERR_OK;
	}
	if((req == 0) && (current_mode != 0)) {
		xil_printf("Initializing test bar.\r\n");
		current_mode = 0;
		InitTestBar(ov7670_test_bar);
	}
	if((req == 1) && (current_mode != 1)) {
		xil_printf("Initializing image capture.\r\n");
		current_mode = 1;
		InitImage(ov7670_image);
	}
	if((req == 2) && (current_mode != 2)) {
		xil_printf("Initializing shifting ones.\r\n");
		current_mode = 2;
		InitTestBar(ov7670_test_shift);
	}

	vdma_ss_get_frame(&vdma);
	xil_printf("VDMA done.\r\n");

	transformYUV422ImageToYUV();
	xil_printf("YUV422 to YUV transform done.\r\n");

	os = make_jpg_image(&image, WIDTH, HEIGHT);
	xil_printf("JPEG coding done.\r\n");

	if (buffer.offset >= os->length) {
			tcp_close(tpcb);
			buffer.offset = 0;
			return ERR_OK;
	}

	if (buffer.offset >= os->length - BUFFER_SIZE) {
		sizeToSend = os->length - buffer.offset;
	} else {
		sizeToSend = BUFFER_SIZE;
	}

	fill_buffer(buffer.buf, os->stream, buffer.offset, sizeToSend);

	tcp_write(tpcb, (void*) buffer.buf, sizeToSend, 0);

	buffer.offset += sizeToSend;

	/* free the received pbuf */
	pbuf_free(p);

	return ERR_OK;
}


int transfer_data() {
	return 0;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err)
{
	static int connection = 1;

	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);
	/* set the send callback for this connection */
	tcp_sent(newpcb, sent_callback);

	/* just use an integer number indicating the connection id as the
	   callback argument */
	tcp_arg(newpcb, (void*)(UINTPTR)connection);

	/* increment for subsequent accepted connections */
	connection++;

	return ERR_OK;
}

int start_application()
{
	struct tcp_pcb *pcb;
	err_t err;
	uint16_t port = 7777;

	/* create new TCP PCB structure */
	pcb = tcp_new();
	if (!pcb) {
		xil_printf("Error creating PCB. Out of Memory\n\r");
		return -1;
	}

	/* bind to specified @port */
	err = tcp_bind(pcb, IP_ADDR_ANY, port);
	if (err != ERR_OK) {
		xil_printf("Unable to bind to port %d: err = %d\n\r", port, err);
		return -2;
	}

	/* we do not need any arguments to callback functions */
	tcp_arg(pcb, NULL);

	/* listen for connections */
	pcb = tcp_listen(pcb);
	if (!pcb) {
		xil_printf("Out of memory while tcp_listen\n\r");
		return -3;
	}

	/* specify callback to use for incoming connections */
	tcp_accept(pcb, accept_callback);

	xil_printf("Server started @ port %d\n\r", port);

	return 0;
}