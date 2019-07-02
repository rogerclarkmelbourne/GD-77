/*
 * Copyright (C)2019 Roger Clark. VK3KYY / G4KYF
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
#include "menu/menuSystem.h"
#include "menu/menuUtilityQSOData.h"
#include "fw_trx.h"
#include "fw_codeplug.h"
#include "fw_settings.h"


static void updateScreen();
static void handleEvent(int buttons, int keys, int events);
static void loadChannelData();
struct_codeplugZone_t currentZone;
struct_codeplugChannel_t channelData;
struct_codeplugRxGroup_t rxGroupData;
struct_codeplugContact_t contactData;
int currentIndexInTRxGroup=0;

int menuChannelMode(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		nonVolatileSettings.initialMenuNumber = MENU_CHANNEL_MODE;// This menu.
		codeplugZoneGetDataForIndex(nonVolatileSettings.currentZone,&currentZone);
		loadChannelData();
		currentChannelData = &channelData;
		menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		updateScreen();
	}
	else
	{
		if (events==0)
		{
			// is there an incoming DMR signal
			if (menuDisplayQSODataState != QSO_DISPLAY_IDLE)
			{
				updateScreen();
			}
		}
		else
		{
			handleEvent(buttons, keys, events);
		}
	}
	return 0;
}
uint16_t byteSwap16(uint16_t in)
{
	return ((in &0xff << 8) | (in >>8));
}

static void loadChannelData()
{
	codeplugChannelGetDataForIndex(currentZone.channels[nonVolatileSettings.currentChannelIndexInZone],&channelData);
	trxSetFrequency(bcd2int(channelData.rxFreq)/10);
	trxSetMode((channelData.chMode==0)?RADIO_MODE_ANALOG:RADIO_MODE_DIGITAL);
	trxSetPower(nonVolatileSettings.txPower);
	codeplugRxGroupGetDataForIndex(channelData.rxGroupList,&rxGroupData);
	codeplugContactGetDataForIndex(rxGroupData.contacts[currentIndexInTRxGroup],&contactData);
	if (settingsIsTgOverride==false)
	{
		trxTalkGroup = contactData.tgNumber;
	}
}

static void updateScreen()
{
	char nameBuf[17];

	UC1701_clearBuf();

	menuUtilityRenderHeader();

	switch(menuDisplayQSODataState)
	{
		case QSO_DISPLAY_DEFAULT_SCREEN:
			codeplugUtilConvertBufToString(channelData.name,nameBuf,16);
			UC1701_printCentered(20, (char *)nameBuf,UC1701_FONT_GD77_8x16);
			if (settingsIsTgOverride)
			{
				sprintf(nameBuf,"TG %d",trxTalkGroup);
			}
			else
			{
				codeplugUtilConvertBufToString(contactData.name,nameBuf,16);
			}
			UC1701_printCentered(40, (char *)nameBuf,UC1701_FONT_GD77_8x16);
			displayLightTrigger();
			UC1701_render();
			break;

		case QSO_DISPLAY_CALLER_DATA:
			menuUtilityRenderQSOData();
			displayLightTrigger();
			UC1701_render();
			break;
	}

	menuDisplayQSODataState = QSO_DISPLAY_IDLE;
}

static void handleEvent(int buttons, int keys, int events)
{
	if ((keys & KEY_GREEN)!=0)
	{
		menuSystemPushNewMenu(MENU_MAIN_MENU);
		return;
	}
	else if ((keys & KEY_HASH)!=0)
	{
		menuSystemPushNewMenu(MENU_NUMERICAL_ENTRY);
		return;
	}
	else if ((keys & KEY_RED)!=0)
	{
		menuSystemSetCurrentMenu(MENU_VFO_MODE);
		return;
	}


	if ((keys & KEY_RIGHT)!=0)
	{
		currentIndexInTRxGroup++;
		if (currentIndexInTRxGroup > (rxGroupData.NOT_IN_MEMORY_numTGsInGroup -1))
		{
			currentIndexInTRxGroup =  0;
		}
		codeplugContactGetDataForIndex(rxGroupData.contacts[currentIndexInTRxGroup],&contactData);

		settingsIsTgOverride=false;
		trxTalkGroup = contactData.tgNumber;

		menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		updateScreen();
	}
	else if ((keys & KEY_LEFT)!=0)
	{
		// To Do change TG in on same channel freq
		currentIndexInTRxGroup--;
		if (currentIndexInTRxGroup < 0)
		{
			currentIndexInTRxGroup =  rxGroupData.NOT_IN_MEMORY_numTGsInGroup - 1;
		}

		codeplugContactGetDataForIndex(rxGroupData.contacts[currentIndexInTRxGroup],&contactData);
		settingsIsTgOverride=false;
		trxTalkGroup = contactData.tgNumber;

		menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		updateScreen();
	}
	else if ((keys & KEY_STAR)!=0)
	{
		if (trxGetMode() == RADIO_MODE_ANALOG)
		{
			trxSetMode(RADIO_MODE_DIGITAL);
		}
		else
		{
			trxSetMode(RADIO_MODE_ANALOG);
		}
		menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		updateScreen();
	}
	else if ((keys & KEY_DOWN)!=0)
	{
		nonVolatileSettings.currentChannelIndexInZone--;
		if (nonVolatileSettings.currentChannelIndexInZone < 0)
		{
			nonVolatileSettings.currentChannelIndexInZone =  currentZone.NOT_IN_MEMORY_numChannelsInZone - 1;
		}
		loadChannelData();
		menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		updateScreen();
	}
	else if ((keys & KEY_UP)!=0)
	{
		nonVolatileSettings.currentChannelIndexInZone++;
		if (nonVolatileSettings.currentChannelIndexInZone > currentZone.NOT_IN_MEMORY_numChannelsInZone - 1)
		{
			nonVolatileSettings.currentChannelIndexInZone = 0;
		}
		loadChannelData();
		menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		updateScreen();
	}
}
