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

#include "fw_trx.h"
#include "fw_HR-C6000.h"
#include "fw_settings.h"

bool open_squelch=false;
bool HR_C6000_datalogging=false;

int trx_measure_count = 0;
bool trxIsTransmitting = false;
uint32_t trxTalkGroup=9;// Set to local TG just in case there is some problem with it not being loaded
uint32_t trxDMRID = 0;// Set ID to 0. Not sure if its valid. This value needs to be loaded from the codeplug.
const int RADIO_VHF_MIN			=	1340000;
const int RADIO_VHF_MAX			=	1740000;
const int RADIO_UHF_MIN			=	4000000;
const int RADIO_UHF_MAX			=	5200000;

static int currentMode = RADIO_MODE_NONE;
static int currentFrequency =1440000;
static uint8_t squelch = 0x00;
static const uint8_t SQUELCH_SETTINGS[] = {45,45,45};


int	trxGetMode()
{
	return currentMode;
}

void trxSetMode(int theMode)
{
	if (theMode != currentMode)
	{
		currentMode=theMode;

		if (currentMode == RADIO_MODE_ANALOG)
		{
			write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x58, 0xBC, 0x7D); // Enable some filters for FM e.g. de-emphasis / pre-emphasis
			write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x44, 0x06, 0x80); // set internal volume to 50%
			GPIO_PinWrite(GPIO_RX_audio_mux, Pin_RX_audio_mux, 1); // connect AT1846S audio to speaker
			terminate_sound();
			terminate_digital();
		}
		else
		{
			write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x58, 0xBC, 0xFD); // Disable all filters in DMR mode
			write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x44, 0x06, 0xCC); // set internal volume to 80%
			GPIO_PinWrite(GPIO_RX_audio_mux, Pin_RX_audio_mux, 0); // connect AT1846S audio to HR_C6000
			init_sound();
			init_digital();
		}
	}
}

static bool check_frequency_is_VHF(int frequency)
{
	return ((frequency >= RADIO_VHF_MIN) && (frequency < RADIO_VHF_MAX));
}

static bool check_frequency_is_UHF(int frequency)
{
	return ((frequency >= RADIO_UHF_MIN) && (frequency < RADIO_UHF_MAX));
}

void trx_check_analog_squelch()
{
	trx_measure_count++;
	if (trx_measure_count==50)
	{
		uint8_t RX_signal;
		uint8_t RX_noise;
		read_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x1b,&RX_signal,&RX_noise);

		if ((RX_noise < SQUELCH_SETTINGS[0]) || (open_squelch))
		{
			GPIO_PinWrite(GPIO_speaker_mute, Pin_speaker_mute, 1); // speaker on
		}
		else
		{
			GPIO_PinWrite(GPIO_speaker_mute, Pin_speaker_mute, 0); // speaker off
		}

    	trx_measure_count=0;
	}
}

void trxSetFrequency(int frequency)
{
	if (currentFrequency!=frequency)
	{
		currentFrequency=frequency;

		if ((currentMode == RADIO_MODE_ANALOG) && (!open_squelch))
		{
			squelch = 0x08;
		}
		else
		{
			squelch = 0x00;
		}

		uint32_t f = currentFrequency * 1.6f;
		uint8_t fl_l = (f & 0x000000ff) >> 0;
		uint8_t fl_h = (f & 0x0000ff00) >> 8;
		uint8_t fh_l = (f & 0x00ff0000) >> 16;
		uint8_t fh_h = (f & 0xff000000) >> 24;

		write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x40, 0x06 | squelch); // RX off
		write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x05, 0x87, 0x63); // select 'normal' frequency mode
		write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x29, fh_h, fh_l);
		write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x2a, fl_h, fl_l);
		write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x49, 0x0C, 0x15); // setting SQ open and shut threshold
		write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x40, 0x26 | squelch); // RX on

		if (check_frequency_is_VHF(currentFrequency))
		{
			GPIO_PinWrite(GPIO_VHF_RX_amp_power, Pin_VHF_RX_amp_power, 1);
			GPIO_PinWrite(GPIO_UHF_RX_amp_power, Pin_UHF_RX_amp_power, 0);
		}
		else if (check_frequency_is_UHF(currentFrequency))
		{
			GPIO_PinWrite(GPIO_VHF_RX_amp_power, Pin_VHF_RX_amp_power, 0);
			GPIO_PinWrite(GPIO_UHF_RX_amp_power, Pin_UHF_RX_amp_power, 1);
		}
	}
}

int trxGetFrequency()
{
	return currentFrequency;
}

void trxSetFrequencyAndMode(int frequency,int mode)
{
	trxSetMode(mode);
	trxSetFrequency(frequency);
}

void trx_setRX()
{
	trxIsTransmitting=false;
	// MUX for RX
	trxSetMode(currentMode);
	GPIO_PinWrite(GPIO_TX_audio_mux, Pin_TX_audio_mux, 0);

	// RX Antenna + PA power off
    DAC_SetBufferValue(DAC0, 0U, 0U);
    GPIO_PinWrite(GPIO_RF_ant_switch, Pin_RF_ant_switch, 0);

	// TX preamp off
	GPIO_PinWrite(GPIO_VHF_TX_amp_power, Pin_VHF_TX_amp_power, 0);
	GPIO_PinWrite(GPIO_UHF_TX_amp_power, Pin_UHF_TX_amp_power, 0);

	// AT1846 RX + unmute
	set_clear_I2C_reg_2byte_with_mask(0x30, 0xFF, 0x1F, 0x00, 0x00);
	set_clear_I2C_reg_2byte_with_mask(0x30, 0xFF, 0x1F, 0x00, 0x20); // RX

	// RX amp on
	if (check_frequency_is_VHF(currentFrequency))
	{
		GPIO_PinWrite(GPIO_VHF_RX_amp_power, Pin_VHF_RX_amp_power, 1);
		GPIO_PinWrite(GPIO_UHF_RX_amp_power, Pin_UHF_RX_amp_power, 0);
	}
	else if (check_frequency_is_UHF(currentFrequency))
	{
		GPIO_PinWrite(GPIO_VHF_RX_amp_power, Pin_VHF_RX_amp_power, 0);
		GPIO_PinWrite(GPIO_UHF_RX_amp_power, Pin_UHF_RX_amp_power, 1);
	}
}

void trx_setTX()
{
	// MUX for TX
	trxIsTransmitting=true;
	trxSetMode(currentMode);
	if (currentMode == RADIO_MODE_ANALOG)
	{
		GPIO_PinWrite(GPIO_TX_audio_mux, Pin_TX_audio_mux, 0);
	}
	else
	{
		GPIO_PinWrite(GPIO_TX_audio_mux, Pin_TX_audio_mux, 1);
	}

	// RX amp off
	GPIO_PinWrite(GPIO_VHF_RX_amp_power, Pin_VHF_RX_amp_power, 0);
	GPIO_PinWrite(GPIO_UHF_RX_amp_power, Pin_UHF_RX_amp_power, 0);

	// AT1846 TX + mute
	set_clear_I2C_reg_2byte_with_mask(0x30, 0xFF, 0x1F, 0x00, 0x00);
	if (currentMode == RADIO_MODE_ANALOG)
	{
		set_clear_I2C_reg_2byte_with_mask(0x30, 0xFF, 0x1F, 0x00, 0x40); // analog TX
	}
	else
	{
		set_clear_I2C_reg_2byte_with_mask(0x30, 0xFF, 0x1F, 0x00, 0xC0); // digital TX
	}

	// TX preamp on
	if (check_frequency_is_VHF(currentFrequency))
	{
		GPIO_PinWrite(GPIO_VHF_TX_amp_power, Pin_VHF_TX_amp_power, 1);
		GPIO_PinWrite(GPIO_UHF_TX_amp_power, Pin_UHF_TX_amp_power, 0);
	}
	else if (check_frequency_is_UHF(currentFrequency))
	{
		GPIO_PinWrite(GPIO_VHF_TX_amp_power, Pin_VHF_TX_amp_power, 0);
		GPIO_PinWrite(GPIO_UHF_TX_amp_power, Pin_UHF_TX_amp_power, 1);
	}

	// TX Antenna + PA power o
    GPIO_PinWrite(GPIO_RF_ant_switch, Pin_RF_ant_switch, 1);
    DAC_SetBufferValue(DAC0, 0U, nonVolatileSettings.txPower);
}

void trxSetPower(uint32_t powerVal)
{
	if (powerVal<4096)
	{
		nonVolatileSettings.txPower = powerVal;
	}
    DAC_SetBufferValue(DAC0, 0U, nonVolatileSettings.txPower);
}
uint16_t trxGetPower()
{
	return nonVolatileSettings.txPower;
}
