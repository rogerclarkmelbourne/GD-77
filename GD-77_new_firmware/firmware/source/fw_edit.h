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

#ifndef _FW_EDIT_H_
#define _FW_EDIT_H_

#include "fw_common.h"
#include "fw_keyboard.h"
#include "fw_display.h"

#include "UC1701.h"

#include "fw_main.h"
#include "fw_sound.h"

#define MODE_SILENT 0
#define MODE_ANALOG 1
#define MODE_DIGITAL 2

#define BAND_VHF 1
#define BAND_UHF 2

#define VHF_MIN 1440000
#define VHF_MAX 1479999
#define UHF_MIN 4300000
#define UHF_MAX 4499999

#define FREQ_STEP 125

extern int current_mode;
extern int current_band;
extern int current_VHF_frequency;
extern int current_UHF_frequency;
extern int current_frequency;

void reset_freq_enter_digits();
int read_freq_enter_digits();
bool check_frequency(int tmp_band, int tmp_frequency);
void update_band_and_frequency(int tmp_band, int tmp_frequency);
void update_screen();
void init_edit();
void tick_edit();

#endif /* _FW_EDIT_H_ */
