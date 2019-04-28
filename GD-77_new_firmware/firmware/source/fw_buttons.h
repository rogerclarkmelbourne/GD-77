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

#ifndef _FW_BUTTONS_H_
#define _FW_BUTTONS_H_

#include "fw_common.h"

#define Port_PTT		PORTA
#define GPIO_PTT		GPIOA
#define Pin_PTT			1
#define Port_SK1		PORTB
#define GPIO_SK1		GPIOB
#define Pin_SK1			1
#define Port_SK2		PORTB
#define GPIO_SK2		GPIOB
#define Pin_SK2			9
#define Port_Orange		PORTA
#define GPIO_Orange		GPIOA
#define Pin_Orange		2

#define BUTTON_PTT    0x00000001
#define BUTTON_SK1    0x00000002
#define BUTTON_SK2    0x00000004
#define BUTTON_ORANGE 0x00000008

#define EVENT_BUTTON_NONE   0
#define EVENT_BUTTON_CHANGE 1

void fw_init_buttons();
uint32_t fw_read_buttons();
void fw_check_button_event(uint32_t *buttons, int *event);

#endif /* _FW_BUTTONS_H_ */
