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

#include "fw_AT1846S.h"

void I2C_AT1846S_init()
{
	// --- start of AT1846_init()
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x00, 0x01); // Soft reset
	vTaskDelay(portTICK_PERIOD_MS * 50);
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x00, 0x04); // Poweron 1846s
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x04, 0x0F, 0xD0); // Clock mode 25.6MHz/26MHz

	// from data table at 0x22f18
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x1F, 0x10, 0x00); // gpio6 = sq out, all others are hi-z
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x09, 0x03, 0xAC); // LDO regulator control (same in GD-77 as UV-82)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x24, 0x00, 0x01); // PLL control - uses default value (PLL lock detection etc)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x31, 0x00, 0x31); // UNDOCUMENTED - use recommended value
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x33, 0x45, 0xF5); // agc number (recommended value)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x34, 0x2B, 0x89); // Rx digital gain (recommend value)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x3F, 0x32, 0x63); // This register is not in the list and defaults to 0x23C6
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x40, 0x00, 0x31); // THIS IS THE MAGIC REGISTER WHICH ALLOWS LOW FREQ AUDIO BY SETTING THE LS BIT
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x41, 0x47, 0x0F); // Digital voice gain, (bits 6:0) however default value is supposed to be 0x4006 hence some bits are being set outside the documented range
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x42, 0x10, 0x36); // RDA1846 lists this as Vox Shut threshold
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x43, 0x00, 0xBB); // FM deviation
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x44, 0x06, 0xFF); // Rx and tx gain controls
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x47, 0x7F, 0x2F); // UNDOCUMENTED - UV82 and GD77 use the same values
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x4E, 0x00, 0x82); // UNDOCUMENTED - use recommended value
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x4F, 0x2C, 0x62); // UNDOCUMENTED - use recommended value
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x53, 0x00, 0x94); // UNDOCUMENTED - use recommended value
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x54, 0x2A, 0x3C); // UNDOCUMENTED - use recommended value
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x55, 0x00, 0x81); // UNDOCUMENTED - use recommended value
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x56, 0x0B, 0x02); // SQ detection time (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x57, 0x1C, 0x00); // bypass rssi_lpfilter
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x5A, 0x49, 0x35); // SQ detection time (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x58, 0xBC, 0xCD); // Filters custom setting
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x62, 0x32, 0x63); // modu_det_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x4E, 0x20, 0x82); // UNDOCUMENTED - use recommended value
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x63, 0x16, 0xAD); // Pre_emphasis bypass threshold (recommended value)
	// end data table at 0x22f18

	// Calibration start
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x40, 0xA4); // Setup to calibrate
	vTaskDelay(portTICK_PERIOD_MS * 50);
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x40, 0xA6); // chip_cal_en Enable calibration
	vTaskDelay(portTICK_PERIOD_MS * 100);
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x40, 0x06); // chip_cal_en Disable calibration
	vTaskDelay(portTICK_PERIOD_MS * 10);
	// Calibration end
	// --- end of AT1846_init()

	// --- start of AT1486_init_mode()
	// --- Init for 12.5KHz
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x15, 0x11, 0x00); // IF tuning bits (12:9)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x32, 0x44, 0x95); // agc target power
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x3A, 0x40, 0xC3); // modu_det_sel (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x3F, 0x28, 0xD0); // Rssi3_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x3C, 0x0F, 0x1E); // Pk_det_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x48, 0x1D, 0xB6); // noise1_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x62, 0x14, 0x25); // modu_det_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x65, 0x24, 0x94); // setting th_sif for SQ rssi detect
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x66, 0xEB, 0x2E); // rssi_comp  and afc range
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x7F, 0x00, 0x01); // Goto page 1 registers
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x06, 0x00, 0x14); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x07, 0x02, 0x0C); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x08, 0x02, 0x14); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x09, 0x03, 0x0C); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0A, 0x03, 0x14); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0B, 0x03, 0x24); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0C, 0x03, 0x44); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0D, 0x13, 0x44); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0E, 0x1B, 0x44); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0F, 0x3F, 0x44); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x12, 0xE0, 0xEB); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x7F, 0x00, 0x00); // Go back to page 0 registers
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x40, 0x06); // filter_band_sel + band_mode_sel = 12.5KHz
	// --- end of AT1486_init_mode()

	/*
	// --- start of AT1486_init_mode()
	// --- Init for 25.0KHz
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x15, 0x1F, 0x00); // IF tuning bits (12:9)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x32, 0x75, 0x64); // agc target power
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x3A, 0x44, 0xC3); // modu_det_sel (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x3F, 0x29, 0xD2); // Rssi3_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x3C, 0x0E, 0x1C); // Pk_det_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x48, 0x1E, 0x38); // noise1_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x62, 0x37, 0x67); // modu_det_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x65, 0x24, 0x8A); // setting th_sif for SQ rssi detect
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x66, 0xFF, 0x2E); // rssi_comp  and afc range
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x7F, 0x00, 0x01); // Goto page 1 registers
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x06, 0x00, 0x24); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x07, 0x02, 0x14); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x08, 0x02, 0x24); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x09, 0x03, 0x14); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0A, 0x03, 0x24); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0B, 0x03, 0x44); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0C, 0x03, 0x84); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0D, 0x13, 0x84); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0E, 0x1B, 0x84); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0F, 0x3F, 0x84); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x12, 0xE0, 0xEB); // AGC Table (recommended value for 12.5kHz bandwidth operation)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x7F, 0x00, 0x00); // Go back to page 0 registers
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x30, 0x70, 0x06); // filter_band_sel + band_mode_sel = 12.5KHz
	// --- end of AT1486_init_mode()
    */

	set_clear_I2C_reg_2byte_with_mask(0x4e, 0xff, 0x3f, 0x00, 0x80); // Select cdcss mode for tx
	vTaskDelay(portTICK_PERIOD_MS * 200);
}

void I2C_AT1846_Postinit()
{
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x58, 0xBC, 0xFD); // Filters custom setting
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0A, 0x7B, 0xA0); // AGC Table
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x41, 0x47, 0x31); // Digital voice gain, (bits 6:0) however default value is supposed to be 0x4006 hence some bits are being set outside the documented range
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x44, 0x05, 0xFF); // Rx and tx gain controls
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x59, 0x09, 0xD2); // Tx FM devitation
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x44, 0x05, 0xCF); // Rx and tx gain controls
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x44, 0x05, 0xCC); // Rx and tx gain controls
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x48, 0x1A, 0x32); // noise1_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x60, 0x1A, 0x32); // noise2_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x3F, 0x29, 0xD1); // Rssi3_th (SQ setting)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x0A, 0x7B, 0xA0); // AGC Table
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x49, 0x0C, 0x96); // setting SQ open and shut threshold
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x33, 0x45, 0xF5); // agc number (recommended value)
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x41, 0x47, 0x0F); // Digital voice gain, (bits 6:0) however default value is supposed to be 0x4006 hence some bits are being set outside the documented range
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x42, 0x10, 0x36); // RDA1846 lists this as Vox Shut threshold
	write_I2C_reg_2byte(I2C_MASTER_SLAVE_ADDR_7BIT, 0x43, 0x00, 0xBB); // FM deviation
}
