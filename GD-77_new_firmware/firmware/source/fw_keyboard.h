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

#ifndef _FW_KEYBOARD_H_
#define _FW_KEYBOARD_H_

#include "fw_common.h"

// column lines
#define Port_Key_Col0   PORTC
#define GPIO_Key_Col0 	GPIOC
#define Pin_Key_Col0	0
#define Port_Key_Col1   PORTC
#define GPIO_Key_Col1 	GPIOC
#define Pin_Key_Col1 	1
#define Port_Key_Col2   PORTC
#define GPIO_Key_Col2 	GPIOC
#define Pin_Key_Col2 	2
#define Port_Key_Col3   PORTC
#define GPIO_Key_Col3 	GPIOC
#define Pin_Key_Col3 	3

// row lines
#define Port_Key_Row0   PORTB
#define GPIO_Key_Row0 	GPIOB
#define Pin_Key_Row0	19
#define Port_Key_Row1   PORTB
#define GPIO_Key_Row1 	GPIOB
#define Pin_Key_Row1	20
#define Port_Key_Row2   PORTB
#define GPIO_Key_Row2 	GPIOB
#define Pin_Key_Row2	21
#define Port_Key_Row3   PORTB
#define GPIO_Key_Row3 	GPIOB
#define Pin_Key_Row3	22
#define Port_Key_Row4   PORTB
#define GPIO_Key_Row4 	GPIOB
#define Pin_Key_Row4	23

void fw_init_keyboard();

#endif /* _FW_KEYBOARD_H_ */
