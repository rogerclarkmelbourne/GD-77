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

#include "fw_display.h"

void fw_init_display()
{
    PORT_SetPinMux(Port_Display_Light, Pin_Display_Light, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Display_CS, Pin_Display_CS, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Display_RST, Pin_Display_RST, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Display_RS, Pin_Display_RS, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Display_SCK, Pin_Display_SCK, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Display_SDA, Pin_Display_SDA, kPORT_MuxAsGpio);

    GPIO_PinInit(GPIO_Display_Light, Pin_Display_Light, &pin_config_output);
    GPIO_PinInit(GPIO_Display_CS, Pin_Display_CS, &pin_config_output);
    GPIO_PinInit(GPIO_Display_RST, Pin_Display_RST, &pin_config_output);
    GPIO_PinInit(GPIO_Display_RS, Pin_Display_RS, &pin_config_output);
    GPIO_PinInit(GPIO_Display_SCK, Pin_Display_SCK, &pin_config_output);
    GPIO_PinInit(GPIO_Display_SDA, Pin_Display_SDA, &pin_config_output);
}
