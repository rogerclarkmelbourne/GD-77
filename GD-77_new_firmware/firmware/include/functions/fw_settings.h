/*
 * Copyright (C)2019 	Roger Clark, VK3KYY / G4KYF
 * 				and		Kai Ludwig, DG4KLU
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

#ifndef _FW_SETTINGS_H_
#define _FW_SETTINGS_H_

#include "fw_common.h"

extern const int BAND_VHF_MIN;
extern const int BAND_VHF_MAX;
extern const int BAND_UHF_MIN;
extern const int BAND_UHF_MAX;
#define VFO_COUNT 4

typedef struct settingsStruct
{
	int 			magicNumber;
	int				vfoFrequenciesArray[VFO_COUNT];
	int16_t			currentChannelIndexInZone;
	int16_t			currentZone;
	uint8_t			vfoFrequencyStepKhz;// step in Khz times 10
	int8_t			currentVFOIndex;
	uint8_t			backLightTimeout;//0 = never timeout. 1 - 255 time in seconds
	int8_t			displayContrast;
	uint8_t			vfoTrxMode;
	uint8_t			initialMenuNumber;
	int8_t			displayBacklightPercentage;
	uint8_t			displayInverseVideo;
	uint16_t		txPower;
} settingsStruct_t;

extern settingsStruct_t nonVolatileSettings;
extern bool settingsIsTgOverride;

void settingsSaveSettings();
void settingsLoadSettings();
void settingsRestoreDefaultSettings();

#endif
