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
#include "image.h"
#include "lwip/err.h"
#include "lwip/tcp.h"
#if defined (__arm__) || defined (__aarch64__)
#include "xil_printf.h"
#endif
#define TO_BE_SENT os->index
#define BUFFER_SIZE 1024
#define TCPDATA os->stream

unsigned long int sent = 0;

int transfer_data() {
	return 0;
}

void print_app_header() {
	xil_printf("\n\r\n\r-----lwIP TCP echo server ------\n\r");
	xil_printf("TCP packets sent to port 6001 will be echoed back\n\r");
}

void fill_buffer(unsigned char buffer[], u8 data[],
		unsigned long int position, int size) {
	for (int i = 0; i < size; ++i) {
		buffer[i] = data[position + i];
		/*if (i % 3 == 0)
			buffer[i] = (unsigned char) data[position + i / 3].Y;
		else if (i % 3 == 1)
			buffer[i] = (unsigned char) data[position + i / 3].Cb;
		else
			buffer[i] = (unsigned char) data[position + i / 3].Cr;*/
		//xil_printf("%X", buffer[i]);
	}
	//xil_printf("\n");
}

// const int BUFFER_SIZE = 1024;
// const int TO_BE_SENT = 480 * 640 * 3;

err_t sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
	int n;
	unsigned char buffer[BUFFER_SIZE];

	if (sent >= TO_BE_SENT)
	{
		tcp_close(tpcb);
		sent = 0;
		free(os->stream);
		os->index = 0;
		return ERR_OK;
	}

	if (sent >= TO_BE_SENT - BUFFER_SIZE)
		n = TO_BE_SENT - sent;
	else
		n = BUFFER_SIZE;

	fill_buffer(buffer, TCPDATA, sent, n);

	//xil_printf("Sent = %ul, Len = %u, TCP send buffer = %u\n", sent, len, tcp_sndbuf(tpcb));

	tcp_write(tpcb, (void*) buffer, n, 1);

	sent += n;

	/*if (sent >= sizeof(image.ycbcr) - 1024) {
	 tcp_write(tpcb, (void*) image.ycbcr + sent, sizeof(image.ycbcr) - sent, 0); //salje
	 tcp_close(tpcb);
	 sent = 0;
	 return ERR_OK;
	 }
	 if (tcp_sndbuf(tpcb) > 1024) {
	 tcp_write(tpcb, (void*)image.ycbcr + sent, 1024, 0); //salje
	 sent += 1024;
	 }*/
	return ERR_OK;
}

err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
	/* do not read the packet if we are not in ESTABLISHED state */

	int n;
	unsigned char buffer[BUFFER_SIZE];

	if (!p) {
		tcp_close(tpcb);
		tcp_recv(tpcb, NULL);
		return ERR_OK;
	}

	/* indicate that the packet has been received */
	tcp_recved(tpcb, p->len);		// prima


	if ((*(char *) p->payload) == 'g') // casta payload u pointer na char jer je on void
	{
		xil_printf("Primio g!\n");

		//izbrisati ove dvije linije
		getImage();
		os = make_jpg_image(&image, IMAGE_WIDTH, IMAGE_HEIGHT);
		for(int index=0; index<os->index;index++){
			os->stream[index]^=XOR_MASK;
		}
		//---------------------------------------------

		if (sent >= TO_BE_SENT)
			{
				tcp_close(tpcb);
				sent = 0;
				return ERR_OK;
			}

		if (sent >= TO_BE_SENT - BUFFER_SIZE)
			n = TO_BE_SENT - sent;
		else
			n = BUFFER_SIZE;

		fill_buffer(buffer, TCPDATA, sent, n);

		//xil_printf("Sent = %ul, Len = %u, TCP send buffer = %u\n", sent, len, tcp_sndbuf(tpcb));

		tcp_write(tpcb, (void*) buffer, n, 1);

		sent += n;
		/*if (tcp_sndbuf(tpcb) > p->len) {
		 err = tcp_write(tpcb, (void*) image.ycbcr, 1024, 0); //salje
		 sent += 1024;
		 } else
		 xil_printf("no space in tcp_sndbuf\n\r");*/
	}
	xil_printf("Poslao odgovor!\n");

	/* free the received pbuf */
	pbuf_free(p);

	return ERR_OK;
}

err_t accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
	static int connection = 1;

	/* set the receive callback for this connection */
	tcp_recv(newpcb, recv_callback);
	tcp_sent(newpcb, sent_callback);

	/* just use an integer number indicating the connection id as the
	 callback argument */
	tcp_arg(newpcb, (void*) (UINTPTR) connection);

	/* increment for subsequent accepted connections */
	connection++;

	return ERR_OK;
}

int start_application() {
	struct tcp_pcb *pcb;
	err_t err;
	unsigned port = 7;

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

	xil_printf("TCP echo server started @ port %d\n\r", port);

	return 0;
}