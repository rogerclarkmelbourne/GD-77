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

#include "fw_adc.h"

uint32_t adc_channel;
uint32_t adc0_dp0;
uint32_t adc0_dp1;
uint32_t adc0_dp2;
uint32_t adc0_dp3;

void trigger_adc()
{
    adc16_channel_config_t adc16ChannelConfigStruct;

    adc16ChannelConfigStruct.channelNumber = adc_channel;
    adc16ChannelConfigStruct.enableInterruptOnConversionCompleted = true;
    adc16ChannelConfigStruct.enableDifferentialConversion = false;
    ADC16_SetChannelConfig(ADC0, 0, &adc16ChannelConfigStruct);
}

void adc_init()
{
	adc16_config_t adc16ConfigStruct;

	adc_channel = 0;
	adc0_dp0 = 0;
	adc0_dp1 = 0;
	adc0_dp2 = 0;
	adc0_dp3 = 0;

    ADC16_GetDefaultConfig(&adc16ConfigStruct);
    ADC16_Init(ADC0, &adc16ConfigStruct);
    ADC16_EnableHardwareTrigger(ADC0, false);
    ADC16_DoAutoCalibration(ADC0);

    EnableIRQ(ADC0_IRQn);

    trigger_adc();
}

void ADC0_IRQHandler(void)
{
    uint32_t result = ADC16_GetChannelConversionValue(ADC0, 0);

    switch (adc_channel)
    {
    case 0: adc0_dp0 = result;
    		break;
    case 1: adc0_dp1 = result;
    		break;
    case 2: adc0_dp2 = result;
    		break;
    case 3: adc0_dp3 = result;
    		break;
    }

    adc_channel++;
    if (adc_channel>3)
    {
    	adc_channel=0;
    }

    trigger_adc();

    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
    exception return operation might vector to incorrect interrupt */
    __DSB();
}

// result of conversion is rounded voltage*10 as integer
int get_battery_voltage()
{
	return adc0_dp1/41.6f+0.5f;
}
