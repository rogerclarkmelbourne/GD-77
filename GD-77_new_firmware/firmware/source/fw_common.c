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

#include "fw_common.h"

gpio_pin_config_t pin_config_input =
{
	kGPIO_DigitalInput,
	0
};

gpio_pin_config_t pin_config_output =
{
	kGPIO_DigitalOutput,
	0
};

void fw_init_common()
{
    CLOCK_EnableClock(kCLOCK_PortA);
    CLOCK_EnableClock(kCLOCK_PortB);
    CLOCK_EnableClock(kCLOCK_PortC);
    CLOCK_EnableClock(kCLOCK_PortD);
    CLOCK_EnableClock(kCLOCK_PortE);

    // Power On/Off logic
    PORT_SetPinMux(Port_Keep_Power_On, Pin_Keep_Power_On, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_Power_Switch, Pin_Power_Switch, kPORT_MuxAsGpio);

    // Power On/Off logic
    GPIO_PinInit(GPIO_Keep_Power_On, Pin_Keep_Power_On, &pin_config_output);
    GPIO_PinInit(GPIO_Power_Switch, Pin_Power_Switch, &pin_config_input);
}
