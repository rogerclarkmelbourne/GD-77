/*
 * Copyright (C)2019 Kai Ludwig, DG4KLU
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "fw_trx.h"
#include "fw_HR-C6000.h"
#include "fw_settings.h"
#include "fw_calibration.h"
#include "fw_AT1846S.h"

bool open_squelch=false;
bool HR_C6000_datalogging=false;

int trx_measure_count = 0;
bool trxIsTransmitting = false;
uint32_t trxTalkGroup=9;// Set to local TG just in case there is some problem with it not being loaded
uint32_t trxDMRID = 0;// Set ID to 0. Not sure if its valid. This value needs to be loaded from the codeplug.
int txstartdelay = 0;
int txstopdelay = 0;

const int RADIO_VHF_MIN			=	1340000;
const int RADIO_VHF_MAX			=	1740000;
const int RADIO_UHF_MIN			=	4000000;
const int RADIO_UHF_MAX			=	5200000;

static int currentMode = RADIO_MODE_NONE;
static int currentBandWidth=0;
static int currentFrequency =1440000;
static int currentCC =1;
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

		I2C_AT1846_SetMode(currentMode);
		trxUpdateAT1846SCalibration();
		trxUpdateAT1846SCalibration();
		if (currentMode == RADIO_MODE_ANALOG)
		{
			GPIO_PinWrite(GPIO_RX_audio_mux, Pin_RX_audio_mux, 1); // connect AT1846S audio to speaker
			terminate_sound();
			terminate_digital();
		}
		else
		{
			trxSetBandWidth(125);// DMR is always 12.5kHz
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

		trxUpdateC6000Calibration();
		trxUpdateAT1846SCalibration();

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

	// TX Antenna + PA power off
    GPIO_PinWrite(GPIO_RF_ant_switch, Pin_RF_ant_switch, 1);
    DAC_SetBufferValue(DAC0, 0U, nonVolatileSettings.txPower);
}

void trxSetPower(uint32_t powerVal)
{
	if (powerVal<4096)
	{
		nonVolatileSettings.txPower = powerVal;
	}
}

uint16_t trxGetPower()
{
	return nonVolatileSettings.txPower;
}

// Use 125 for 12.5kHz, or 250 for 25kHz
void trxSetBandWidth(int bandWidthkHzx10)
{

	if (currentBandWidth==bandWidthkHzx10)
	{
		return;
	}
	currentBandWidth = bandWidthkHzx10;

	I2C_AT1846_SetBandwidth(bandWidthkHzx10);
}

void trxCalcBandAndFrequencyOffset(int *band_offset, int *freq_offset)
{
	if (check_frequency_is_VHF(currentFrequency))
	{
		*band_offset=0x00000070;
		if (currentFrequency<1380000)
		{
			*freq_offset=0x00000000;
		}
		else if (currentFrequency<1425000)
		{
			*freq_offset=0x00000001;
		}
		else if (currentFrequency<1475000)
		{
			*freq_offset=0x00000002;
		}
		else if (currentFrequency<1525000)
		{
			*freq_offset=0x00000003;
		}
		else if (currentFrequency<1575000)
		{
			*freq_offset=0x00000004;
		}
		else if (currentFrequency<1625000)
		{
			*freq_offset=0x00000005;
		}
		else if (currentFrequency<1685000)
		{
			*freq_offset=0x00000006;
		}
		else
		{
			*freq_offset=0x00000007;
		}
	}
	else if (check_frequency_is_UHF(currentFrequency))
	{
		*band_offset=0x00000000;
		if (currentFrequency<4100000)
		{
			*freq_offset=0x00000000;
		}
		else if (currentFrequency<4200000)
		{
			*freq_offset=0x00000001;
		}
		else if (currentFrequency<4300000)
		{
			*freq_offset=0x00000002;
		}
		else if (currentFrequency<4400000)
		{
			*freq_offset=0x00000003;
		}
		else if (currentFrequency<4500000)
		{
			*freq_offset=0x00000004;
		}
		else if (currentFrequency<4600000)
		{
			*freq_offset=0x00000005;
		}
		else if (currentFrequency<4700000)
		{
			*freq_offset=0x00000006;
		}
		else
		{
			*freq_offset=0x00000007;
		}
	}
}

void trxUpdateC6000Calibration()
{
	int band_offset=0x00000000;
	int freq_offset=0x00000000;

	if (nonVolatileSettings.useCalibration==0)
	{
		return;
	}

	trxCalcBandAndFrequencyOffset(&band_offset, &freq_offset);

	uint8_t val_shift;
	read_val_DACDATA_shift(band_offset,&val_shift);
	write_SPI_page_reg_byte_SPI0(0x04, 0x37, val_shift); // DACDATA shift (LIN_VOL)

	uint8_t val_0x04;
	read_val_Q_MOD2_offset(band_offset,&val_0x04);
	write_SPI_page_reg_byte_SPI0(0x04, 0x04, val_0x04); // MOD2 offset

	uint8_t val_0x46;
	read_val_phase_reduce(band_offset+freq_offset,&val_0x46);
	write_SPI_page_reg_byte_SPI0(0x04, 0x46, val_0x46); // phase reduce

	uint8_t val_0x47;
	uint8_t val_0x48;
	read_val_twopoint_mod(band_offset,&val_0x47, &val_0x48);
	write_SPI_page_reg_byte_SPI0(0x04, 0x48, val_0x48); // bit 0 to 1 = upper 2 bits of 10-bit twopoint mod
	write_SPI_page_reg_byte_SPI0(0x04, 0x47, val_0x47); // bit 0 to 7 = lower 8 bits of 10-bit twopoint mod
}

void I2C_AT1846_set_register_with_mask(uint8_t reg, uint16_t mask, uint16_t value, uint8_t shift)
{
	set_clear_I2C_reg_2byte_with_mask(reg, (mask & 0xff00) >> 8, (mask & 0x00ff) >> 0, ((value << shift) & 0xff00) >> 8, ((value << shift) & 0x00ff) >> 0);
}

void trxUpdateAT1846SCalibration()
{
	int band_offset=0x00000000;
	int freq_offset=0x00000000;

	if (nonVolatileSettings.useCalibration==0)
	{
		return;
	}

	trxCalcBandAndFrequencyOffset(&band_offset, &freq_offset);

	uint8_t val_pga_gain;
	uint8_t voice_gain_tx;
	uint8_t gain_tx;
	uint8_t padrv_ibit;

	uint16_t xmitter_dev_narrowband;

	uint8_t dac_vgain_analog = 0x0C;
	uint8_t volume_analog = 0x0C;

	uint16_t noise1_th_narrowband;
	uint16_t noise2_th_narrowband;
	uint16_t rssi3_th_narrowband;

	uint16_t squelch_th;

	read_val_pga_gain(band_offset, &val_pga_gain);
	read_val_voice_gain_tx(band_offset, &voice_gain_tx);
	read_val_gain_tx(band_offset, &gain_tx);
	read_val_padrv_ibit(band_offset, &padrv_ibit);

	read_val_xmitter_dev_narrowband(band_offset, &xmitter_dev_narrowband);

	read_val_noise1_th_narrowband(band_offset, &noise1_th_narrowband);
	read_val_noise2_th_narrowband(band_offset, &noise2_th_narrowband);
	read_val_rssi3_th_narrowband(band_offset, &rssi3_th_narrowband);

	read_val_squelch_th(band_offset+freq_offset, &squelch_th);

	I2C_AT1846_set_register_with_mask(0x0A, 0xF83F, val_pga_gain, 6);
	I2C_AT1846_set_register_with_mask(0x41, 0xFF80, voice_gain_tx, 0);
	I2C_AT1846_set_register_with_mask(0x44, 0xF0FF, gain_tx, 8);

	I2C_AT1846_set_register_with_mask(0x59, 0x003f, xmitter_dev_narrowband, 6);
	I2C_AT1846_set_register_with_mask(0x44, 0xFF0F, dac_vgain_analog, 4);
	I2C_AT1846_set_register_with_mask(0x44, 0xFFF0, volume_analog, 0);

	I2C_AT1846_set_register_with_mask(0x48, 0x0000, noise1_th_narrowband, 0);
	I2C_AT1846_set_register_with_mask(0x60, 0x0000, noise2_th_narrowband, 0);
	I2C_AT1846_set_register_with_mask(0x3f, 0x0000, rssi3_th_narrowband, 0);

	I2C_AT1846_set_register_with_mask(0x0A, 0x87FF, padrv_ibit, 11);

	I2C_AT1846_set_register_with_mask(0x49, 0x0000, squelch_th, 0);
}

void trxSetDMRColourCode(int colourCode)
{
	write_SPI_page_reg_byte_SPI0(0x04, 0x1F, (colourCode << 4)); // DMR Colour code in upper 4 bits.
	currentCC = colourCode;
}

int trxGetDMRColourCode()
{
	return currentCC;
}
