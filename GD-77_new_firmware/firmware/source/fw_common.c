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

    // Power On/Off logic
	GPIO_PinWrite(GPIO_Keep_Power_On, Pin_Keep_Power_On, 1);

    // SOFT_SPI to W25Q80BV 1M flash
    PORT_SetPinMux(Port_SOFT_SPI_CS_FLASH, Pin_SOFT_SPI_CS_FLASH, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_SOFT_SPI_CLK_FLASH, Pin_SOFT_SPI_CLK_FLASH, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_SOFT_SPI_DI_FLASH, Pin_SOFT_SPI_DI_FLASH, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_SOFT_SPI_DO_FLASH, Pin_SOFT_SPI_DO_FLASH, kPORT_MuxAsGpio);
    GPIO_PinInit(GPIO_SOFT_SPI_CS_FLASH, Pin_SOFT_SPI_CS_FLASH, &pin_config_output);
    GPIO_PinInit(GPIO_SOFT_SPI_CLK_FLASH, Pin_SOFT_SPI_CLK_FLASH, &pin_config_output);
    GPIO_PinInit(GPIO_SOFT_SPI_DI_FLASH,Pin_SOFT_SPI_DI_FLASH, &pin_config_output);
    GPIO_PinInit(GPIO_SOFT_SPI_DO_FLASH, Pin_SOFT_SPI_DO_FLASH, &pin_config_input);
    GPIO_PinWrite(GPIO_SOFT_SPI_CS_FLASH, Pin_SOFT_SPI_CS_FLASH, 1);
    GPIO_PinWrite(GPIO_SOFT_SPI_CLK_FLASH, Pin_SOFT_SPI_CLK_FLASH, 0);
    GPIO_PinWrite(GPIO_SOFT_SPI_DI_FLASH,Pin_SOFT_SPI_DI_FLASH, 1);

    // Speaker mute and RX/TX mux init
    PORT_SetPinMux(Port_speaker_mute, Pin_speaker_mute, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_RX_audio_mux, Pin_RX_audio_mux, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_TX_audio_mux, Pin_TX_audio_mux, kPORT_MuxAsGpio);
    GPIO_PinInit(GPIO_speaker_mute, Pin_speaker_mute, &pin_config_output);
    GPIO_PinInit(GPIO_RX_audio_mux, Pin_RX_audio_mux, &pin_config_output);
    GPIO_PinInit(GPIO_TX_audio_mux, Pin_TX_audio_mux, &pin_config_output);
    GPIO_PinWrite(GPIO_speaker_mute, Pin_speaker_mute, 0);
    GPIO_PinWrite(GPIO_RX_audio_mux, Pin_RX_audio_mux, 0);
    GPIO_PinWrite(GPIO_TX_audio_mux, Pin_TX_audio_mux, 0);

    // Antenna switch and UHF/VHF RX/TX amp init
    PORT_SetPinMux(Port_RF_ant_switch, Pin_RF_ant_switch, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_VHF_RX_amp_power, Pin_VHF_RX_amp_power, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_UHF_RX_amp_power, Pin_UHF_RX_amp_power, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_UHF_TX_amp_power, Pin_UHF_TX_amp_power, kPORT_MuxAsGpio);
    PORT_SetPinMux(Port_VHF_TX_amp_power, Pin_VHF_TX_amp_power, kPORT_MuxAsGpio);
    GPIO_PinInit(GPIO_RF_ant_switch, Pin_RF_ant_switch, &pin_config_output);
    GPIO_PinInit(GPIO_VHF_RX_amp_power, Pin_VHF_RX_amp_power, &pin_config_output);
    GPIO_PinInit(GPIO_UHF_RX_amp_power, Pin_UHF_RX_amp_power, &pin_config_output);
    GPIO_PinInit(GPIO_UHF_TX_amp_power, Pin_UHF_TX_amp_power, &pin_config_output);
    GPIO_PinInit(GPIO_VHF_TX_amp_power, Pin_VHF_TX_amp_power, &pin_config_output);
    GPIO_PinWrite(GPIO_RF_ant_switch, Pin_RF_ant_switch, 0);
    GPIO_PinWrite(GPIO_VHF_RX_amp_power, Pin_VHF_RX_amp_power, 0);
    GPIO_PinWrite(GPIO_UHF_RX_amp_power, Pin_UHF_RX_amp_power, 0);
    GPIO_PinWrite(GPIO_UHF_TX_amp_power, Pin_UHF_TX_amp_power, 0);
    GPIO_PinWrite(GPIO_VHF_TX_amp_power, Pin_VHF_TX_amp_power, 0);
}
