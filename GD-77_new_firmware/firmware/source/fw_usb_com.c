/*
 * Copyright (C)2019 Kai Ludwig, DG4KLU
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fw_usb_com.h"

bool int_sys;
bool int_ts;

uint8_t com_buffer[COM_BUFFER_SIZE];
int com_buffer_write_idx = 0;
int com_buffer_read_idx = 0;
int com_buffer_cnt = 0;

void PORTC_IRQHandler(void)
{
    if ((1U << Pin_INT_C6000_SYS) & PORT_GetPinsInterruptFlags(Port_INT_C6000_SYS))
    {
    	int_sys=true;
        PORT_ClearPinsInterruptFlags(Port_INT_C6000_SYS, (1U << Pin_INT_C6000_SYS));
    }
    if ((1U << Pin_INT_C6000_TS) & PORT_GetPinsInterruptFlags(Port_INT_C6000_TS))
    {
    	int_ts=true;
        PORT_ClearPinsInterruptFlags(Port_INT_C6000_TS, (1U << Pin_INT_C6000_TS));
    }

    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
    exception return operation might vector to incorrect interrupt */
    __DSB();
}

void init_HR_C6000_interrupts()
{
	int_sys=false;
	int_ts=false;

    PORT_SetPinInterruptConfig(Port_INT_C6000_SYS, Pin_INT_C6000_SYS, kPORT_InterruptEitherEdge);
    PORT_SetPinInterruptConfig(Port_INT_C6000_TS, Pin_INT_C6000_TS, kPORT_InterruptEitherEdge);

    NVIC_SetPriority(PORTC_IRQn, 3);
}

void add_to_commbuffer(uint8_t value)
{
	com_buffer[com_buffer_write_idx]=value;
	com_buffer_cnt++;
	com_buffer_write_idx++;
	if (com_buffer_write_idx==COM_BUFFER_SIZE)
	{
		com_buffer_write_idx=0;
	}
}