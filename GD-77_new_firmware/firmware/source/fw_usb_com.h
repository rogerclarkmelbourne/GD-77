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

#ifndef _FW_USB_COM_H_
#define _FW_USB_COM_H_

#include "FreeRTOS.h"
#include "task.h"

#include "fw_common.h"

#define COM_BUFFER_SIZE 8192

extern uint8_t tmp_val_0x82;
extern uint8_t tmp_val_0x86;
extern uint8_t tmp_val_0x51;
extern uint8_t tmp_val_0x52;
extern uint8_t tmp_val_0x57;
extern uint8_t tmp_val_0x5f;
extern uint8_t tmp_ram[256];
extern uint8_t tmp_ram1[256];
extern uint8_t tmp_ram2[256];
extern uint16_t custom_value;

extern uint8_t com_buffer[COM_BUFFER_SIZE];
extern int com_buffer_write_idx;
extern int com_buffer_read_idx;
extern int com_buffer_cnt;

void send_packet(uint8_t val_0x82, uint8_t val_0x86, int ram);
void send_packet_big(uint8_t val_0x82, uint8_t val_0x86, int ram1, int ram2);

void add_to_commbuffer(uint8_t value);

#endif /* _FW_USB_COM_H_ */
