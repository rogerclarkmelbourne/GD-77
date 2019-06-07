/*
 * Copyright (C)2019 	Kai Ludwig, DG4KLU
 * 				and		Roger Clark, VK3KYY / G4KYF
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

#include "fw_settings.h"
#include "fw_EEPROM.h"
#include "fw_trx.h"
#include "menu/menuSystem.h"

const int BAND_VHF_MIN 	= 1440000;
const int BAND_VHF_MAX 	= 1480000;
const int BAND_UHF_MIN 	= 4300000;
const int BAND_UHF_MAX 	= 4500000;

static const int STORAGE_BASE_ADDRESS = 0xFF00;
static const int STORAGE_MAGIC_NUMBER = 0x4714;

settingsStruct_t nonVolatileSettings;

void settingsSaveSettings()
{
	EEPROM_Write(STORAGE_BASE_ADDRESS, (uint8_t*)&nonVolatileSettings, sizeof(settingsStruct_t));
    vTaskDelay(portTICK_PERIOD_MS * 5);
}

void settingsLoadSettings()
{
	EEPROM_Read(STORAGE_BASE_ADDRESS, (uint8_t*)&nonVolatileSettings, sizeof(settingsStruct_t));
	if (nonVolatileSettings.magicNumber != STORAGE_MAGIC_NUMBER)
	{
    	settingsRestoreDefaultSettings();
    	settingsSaveSettings();
	}
}

void settingsRestoreDefaultSettings()
{
	nonVolatileSettings.magicNumber=STORAGE_MAGIC_NUMBER;
	for(int i=0;i<4;i++)
	{
		nonVolatileSettings.vfoFrequenciesArray[i]=	BAND_VHF_MIN;// Note. Need to change this hard coded frequency
	}

	nonVolatileSettings.currentChannelIndexInZone = 0;
	nonVolatileSettings.currentZone = 0;
	nonVolatileSettings.vfoFrequencyStepKhz = 125;// 12.5 step in Khz times 10
	nonVolatileSettings.currentVFOIndex = 0 ;
	nonVolatileSettings.backLightTimeout = 5;//0 = never timeout. 1 - 255 time in seconds
	nonVolatileSettings.displayContrast = 0x0E;
	nonVolatileSettings.vfoTrxMode = RADIO_MODE_DIGITAL;
	nonVolatileSettings.initialMenuNumber=MENU_VFO_MODE;
	nonVolatileSettings.displayBacklightPercentage=100U;// 100% brightness
	nonVolatileSettings.displayInverseVideo=0;// Not inverse video
}
