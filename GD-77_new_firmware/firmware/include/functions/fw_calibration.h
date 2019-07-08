/*
 * Copyright (C)2019 Roger Clark. VK3KYY / G4KYF
 * 				and  Kai Ludwig, DG4KLU
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
#ifndef _FW_CALIBRATION_H_
#define _FW_CALIBRATION_H_

#include "fw_common.h"
#include "fw_SPI_Flash.h"

typedef struct calibrationStruct
{
	int masterOscillator;
	uint8_t powerLevels[16];
} calibrationStruct_t;

extern calibrationStruct_t calibrationVHF;
extern calibrationStruct_t calibrationUHF;

#define EXT_DACDATA_shift 0x0008F05D
#define EXT_twopoint_mod  0x0008F008
#define EXT_Q_MOD2_offset 0x0008F00A
#define EXT_phase_reduce  0x0008F055

void read_val_DACDATA_shift(int offset, uint8_t* val_shift);
void read_val_twopoint_mod(int offset, uint8_t* val_0x47, uint8_t* val_0x48);
void read_val_Q_MOD2_offset(int offset, uint8_t* val_0x04);
void read_val_phase_reduce(int offset, uint8_t* val_0x46);

#endif
