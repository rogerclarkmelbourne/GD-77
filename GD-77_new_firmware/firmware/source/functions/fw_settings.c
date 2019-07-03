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
#include "fw_codeplug.h"

const int BAND_VHF_MIN 	= 1440000;
const int BAND_VHF_MAX 	= 1480000;
const int BAND_UHF_MIN 	= 4300000;
const int BAND_UHF_MAX 	= 4500000;

static const int STORAGE_BASE_ADDRESS = 0xFF00;
static const int STORAGE_MAGIC_NUMBER = 0x4716;

bool settingsIsTgOverride=false;
settingsStruct_t nonVolatileSettings;
struct_codeplugChannel_t *currentChannelData;

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
	trxDMRID = codeplugGetUserDMRID();
}

void initVFOChannel()
{
	strcpy(nonVolatileSettings.vfoChannel.name,"VFO");
	nonVolatileSettings.vfoChannel.rxFreq=BAND_VHF_MIN;
	nonVolatileSettings.vfoChannel.txFreq=BAND_VHF_MIN;
	nonVolatileSettings.vfoChannel.chMode =  RADIO_MODE_DIGITAL;
	nonVolatileSettings.vfoChannel.rxRefFreq=0;
	nonVolatileSettings.vfoChannel.txRefFreq=0;
	nonVolatileSettings.vfoChannel.tot=180;
	nonVolatileSettings.vfoChannel.totRekey=0;;
	nonVolatileSettings.vfoChannel.admitCriteria = ADMIT_CRITERIA_ALWAYS;
	nonVolatileSettings.vfoChannel.rssiThreshold = 45;
	nonVolatileSettings.vfoChannel.scanList=0;
	nonVolatileSettings.vfoChannel.rxTone=0;
	nonVolatileSettings.vfoChannel.txTone=0;
	nonVolatileSettings.vfoChannel.voiceEmphasis=0;
	nonVolatileSettings.vfoChannel.txSignaling=0;
	nonVolatileSettings.vfoChannel.unmuteRule=0;
	nonVolatileSettings.vfoChannel.rxSignaling=0;
	nonVolatileSettings.vfoChannel.artsInterval=0;
	nonVolatileSettings.vfoChannel.encrypt=0;
	nonVolatileSettings.vfoChannel.rxColor=0;
	nonVolatileSettings.vfoChannel.rxGroupList=0;
	nonVolatileSettings.vfoChannel.txColor=0;
	nonVolatileSettings.vfoChannel.emgSystem=0;
	nonVolatileSettings.vfoChannel.contact=0;
	nonVolatileSettings.vfoChannel.flag1=0;
	nonVolatileSettings.vfoChannel.flag2=0;
	nonVolatileSettings.vfoChannel.flag3=0;
	nonVolatileSettings.vfoChannel.flag4=0;
	nonVolatileSettings.vfoChannel.reserve2=0;
	nonVolatileSettings.vfoChannel.reserve=0;
	nonVolatileSettings.vfoChannel.sql=45;
}

void settingsRestoreDefaultSettings()
{
	nonVolatileSettings.magicNumber=STORAGE_MAGIC_NUMBER;
	nonVolatileSettings.currentChannelIndexInZone = 0;
	nonVolatileSettings.currentZone = 0;
	nonVolatileSettings.backLightTimeout = 5;//0 = never timeout. 1 - 255 time in seconds
	nonVolatileSettings.displayContrast = 0x0E;
	nonVolatileSettings.initialMenuNumber=MENU_VFO_MODE;
	nonVolatileSettings.displayBacklightPercentage=100U;// 100% brightness
	nonVolatileSettings.displayInverseVideo=0;// Not inverse video
	nonVolatileSettings.txPower=1000;
	initVFOChannel();
	currentChannelData = &nonVolatileSettings.vfoChannel;// Set the current channel data to point to the VFO data since the default screen will be the VFO
}
