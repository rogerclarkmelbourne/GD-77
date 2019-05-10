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

#ifndef _FW_SOUND_H_
#define _FW_SOUND_H_

#include "FreeRTOS.h"
#include "task.h"

#include "fw_common.h"
#include "fw_spi.h"

#include "fw_edit.h"

extern int melody_generic[512];
extern int melody_poweron[];
extern int melody_key_beep[];
extern int melody_sk1_beep[];
extern int melody_sk2_beep[];
extern int melody_orange_beep[];
extern int melody_ACK_beep[];
extern int melody_NACK_beep[];
extern int melody_ERROR_beep[];

extern const uint8_t melody1[];
extern const uint8_t melody2[];
extern const uint8_t melody3[];
extern const uint8_t melody4[];

#define WAV_BUFFER_SIZE 0xa0
#define WAV_BUFFER_COUNT 16

extern volatile uint8_t wavbuffer[WAV_BUFFER_COUNT][WAV_BUFFER_SIZE];
extern int wavbuffer_read_idx;
extern int wavbuffer_write_idx;
extern volatile int wavbuffer_count;
extern uint8_t tmp_wavbuffer[WAV_BUFFER_SIZE];

extern uint8_t spi_sound1[WAV_BUFFER_SIZE*2];
extern uint8_t spi_sound2[WAV_BUFFER_SIZE*2];
extern uint8_t spi_sound3[WAV_BUFFER_SIZE*2];
extern uint8_t spi_sound4[WAV_BUFFER_SIZE*2];

extern volatile bool g_TX_SAI_in_use;

void set_melody(int *melody);
int get_freq(int tone);
void create_song(const uint8_t *melody);
void fw_init_beep_task();
void send_sound_data();
void store_soundbuffer();
void tick_soundbuffer();
void tick_melody();
void fw_beep_task();

#endif /* _FW_SOUND_H_ */
