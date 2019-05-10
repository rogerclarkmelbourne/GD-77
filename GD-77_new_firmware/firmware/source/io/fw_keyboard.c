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

#include "fw_keyboard.h"

uint32_t keys;
int key_event;

uint32_t old_keyboard_state;

void fw_init_keyboard()
{
    // column lines
    PORT_SetPinMux(Port_Key_Col0, Pin_Key_Col0, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Key_Col1, Pin_Key_Col1, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Key_Col2, Pin_Key_Col2, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Key_Col3, Pin_Key_Col3, kPORT_MuxAsGpio);

    // row lines
    PORT_SetPinMux(Port_Key_Row0, Pin_Key_Row0, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Key_Row1, Pin_Key_Row1, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Key_Row2, Pin_Key_Row2, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Key_Row3, Pin_Key_Row3, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Key_Row4, Pin_Key_Row4, kPORT_MuxAsGpio);

    // column lines
    GPIO_PinInit(GPIO_Key_Col0, Pin_Key_Col0, &pin_config_input);
    GPIO_PinInit(GPIO_Key_Col1, Pin_Key_Col1, &pin_config_input);
    GPIO_PinInit(GPIO_Key_Col2, Pin_Key_Col2, &pin_config_input);
    GPIO_PinInit(GPIO_Key_Col3, Pin_Key_Col3, &pin_config_input);

    // row lines
    GPIO_PinInit(GPIO_Key_Row0, Pin_Key_Row0, &pin_config_input);
    GPIO_PinInit(GPIO_Key_Row1, Pin_Key_Row1, &pin_config_input);
    GPIO_PinInit(GPIO_Key_Row2, Pin_Key_Row2, &pin_config_input);
    GPIO_PinInit(GPIO_Key_Row3, Pin_Key_Row3, &pin_config_input);
    GPIO_PinInit(GPIO_Key_Row4, Pin_Key_Row4, &pin_config_input);

    old_keyboard_state = 0;
}

uint8_t fw_read_keyboard_col()
{
	uint8_t result=0;
	if (GPIO_PinRead(GPIO_Key_Row0, Pin_Key_Row0)==0)
	{
		result|=0x01;
	}
	if (GPIO_PinRead(GPIO_Key_Row1, Pin_Key_Row1)==0)
	{
		result|=0x02;
	}
	if (GPIO_PinRead(GPIO_Key_Row2, Pin_Key_Row2)==0)
	{
		result|=0x04;
	}
	if (GPIO_PinRead(GPIO_Key_Row3, Pin_Key_Row3)==0)
	{
		result|=0x08;
	}
	if (GPIO_PinRead(GPIO_Key_Row4, Pin_Key_Row4)==0)
	{
		result|=0x10;
	}
	return result;
}

uint32_t fw_read_keyboard()
{
    GPIO_PinInit(GPIO_Key_Col3, Pin_Key_Col3, &pin_config_output);
	GPIO_PinWrite(GPIO_Key_Col3, Pin_Key_Col3, 0);
	uint32_t result=fw_read_keyboard_col();
	GPIO_PinWrite(GPIO_Key_Col3, Pin_Key_Col3, 1);
    GPIO_PinInit(GPIO_Key_Col3, Pin_Key_Col3, &pin_config_input);

    GPIO_PinInit(GPIO_Key_Col2, Pin_Key_Col2, &pin_config_output);
	GPIO_PinWrite(GPIO_Key_Col2, Pin_Key_Col2, 0);
	result=(result<<5)|fw_read_keyboard_col();
	GPIO_PinWrite(GPIO_Key_Col2, Pin_Key_Col2, 1);
    GPIO_PinInit(GPIO_Key_Col2, Pin_Key_Col2, &pin_config_input);

    GPIO_PinInit(GPIO_Key_Col1, Pin_Key_Col1, &pin_config_output);
	GPIO_PinWrite(GPIO_Key_Col1, Pin_Key_Col1, 0);
	result=(result<<5)|fw_read_keyboard_col();
	GPIO_PinWrite(GPIO_Key_Col1, Pin_Key_Col1, 1);
    GPIO_PinInit(GPIO_Key_Col1, Pin_Key_Col1, &pin_config_input);

    GPIO_PinInit(GPIO_Key_Col0, Pin_Key_Col0, &pin_config_output);
	GPIO_PinWrite(GPIO_Key_Col0, Pin_Key_Col0, 0);
	result=(result<<5)|fw_read_keyboard_col();
	GPIO_PinWrite(GPIO_Key_Col0, Pin_Key_Col0, 1);
    GPIO_PinInit(GPIO_Key_Col0, Pin_Key_Col0, &pin_config_input);

    return result;
}

void fw_check_key_event(uint32_t *keys, int *event)
{
	*keys = fw_read_keyboard();

	if (old_keyboard_state!=*keys)
	{
		old_keyboard_state=*keys;
		*event = EVENT_KEY_CHANGE;
	}
	else
	{
		*event = EVENT_KEY_NONE;
	}
}
