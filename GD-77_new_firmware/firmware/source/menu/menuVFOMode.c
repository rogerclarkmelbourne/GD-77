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
#include "fw_settings.h"



enum VFO_SELECTED_FREQUENCY_INPUT  {VFO_SELECTED_FREQUENCY_INPUT_RX , VFO_SELECTED_FREQUENCY_INPUT_TX};

static char freq_enter_digits[7] = { '-', '-', '-', '-', '-', '-', '-' };
static int freq_enter_idx = 0;
static int selectedFreq = VFO_SELECTED_FREQUENCY_INPUT_TX;

static struct_codeplugRxGroup_t rxGroupData;
static struct_codeplugContact_t contactData;

static int FREQ_STEP =	125;// will load from settings
static int currentIndexInTRxGroup=0;

// internal prototypes
static void updateScreen();
static void handleEvent(int buttons, int keys, int events);

static void reset_freq_enter_digits();
static int read_freq_enter_digits();
static bool check_frequency(int tmp_frequency);
static void update_frequency(int tmp_frequency);
static void stepFrequency(int increment);


// public interface
int menuVFOMode(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		nonVolatileSettings.initialMenuNumber=MENU_VFO_MODE;
		currentChannelData = &nonVolatileSettings.vfoChannel;
		trxSetFrequency(currentChannelData->rxFreq);
		trxSetMode(currentChannelData->chMode);
		trxSetPower(nonVolatileSettings.txPower);
		codeplugRxGroupGetDataForIndex(currentChannelData->rxGroupList,&rxGroupData);
		codeplugContactGetDataForIndex(rxGroupData.contacts[currentIndexInTRxGroup],&contactData);
		if (settingsIsTgOverride==false)
		{
			trxTalkGroup = contactData.tgNumber;
		}
		reset_freq_enter_digits();
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

static void updateScreen()
{
	int val_before_dp;
	int val_after_dp;

	char buffer[32];
	UC1701_clearBuf();

	menuUtilityRenderHeader();

	switch(menuDisplayQSODataState)
	{
		case QSO_DISPLAY_DEFAULT_SCREEN:

			if (trxGetMode() == RADIO_MODE_DIGITAL)
			{
				sprintf(buffer,"TG %d",trxTalkGroup);
				UC1701_printCentered(16,buffer,UC1701_FONT_GD77_8x16);
			}

			if (freq_enter_idx==0)
			{
				val_before_dp = currentChannelData->txFreq/10000;
				val_after_dp = currentChannelData->txFreq - val_before_dp*10000;
				sprintf(buffer,"%cT %d.%04d MHz", (selectedFreq == VFO_SELECTED_FREQUENCY_INPUT_TX)?'>':' ',val_before_dp, val_after_dp);
				UC1701_printCentered(32, buffer,UC1701_FONT_GD77_8x16);

				val_before_dp = currentChannelData->rxFreq/10000;
				val_after_dp = currentChannelData->rxFreq - val_before_dp*10000;
				sprintf(buffer,"%cR %d.%04d MHz", (selectedFreq == VFO_SELECTED_FREQUENCY_INPUT_RX)?'>':' ',val_before_dp, val_after_dp);
				UC1701_printCentered(48, buffer,UC1701_FONT_GD77_8x16);
			}
			else
			{
				sprintf(buffer,"%c%c%c.%c%c%c%c MHz", freq_enter_digits[0], freq_enter_digits[1], freq_enter_digits[2], freq_enter_digits[3], freq_enter_digits[4], freq_enter_digits[5], freq_enter_digits[6] );
				if (selectedFreq == VFO_SELECTED_FREQUENCY_INPUT_TX)
				{
					UC1701_printCentered(32, buffer,UC1701_FONT_GD77_8x16);
				}
				else
				{
					UC1701_printCentered(48, buffer,UC1701_FONT_GD77_8x16);
				}
			}

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

static void reset_freq_enter_digits()
{
	for (int i=0;i<7;i++)
	{
		freq_enter_digits[i]='-';
	}
	freq_enter_idx = 0;
}

static int read_freq_enter_digits()
{
	int result=0;
	for (int i=0;i<7;i++)
	{
		result=result*10;
		if ((freq_enter_digits[i]>='0') && (freq_enter_digits[i]<='9'))
		{
			result=result+freq_enter_digits[i]-'0';
		}
	}
	return result;
}

static bool check_frequency(int tmp_frequency)
{
	return ((tmp_frequency>=BAND_VHF_MIN) && (tmp_frequency<=BAND_VHF_MAX)) || ((tmp_frequency>=BAND_UHF_MIN) && (tmp_frequency<=BAND_UHF_MAX));
}

static void update_frequency(int frequency)
{
	if (selectedFreq == VFO_SELECTED_FREQUENCY_INPUT_TX)
	{
		currentChannelData->txFreq=frequency;
	}
	else
	{
		currentChannelData->rxFreq=frequency;
	}
	trxSetFrequency(frequency);
}

static void handleEvent(int buttons, int keys, int events)
{
	if (events & 0x02)
	{
		if (buttons & BUTTON_ORANGE)
		{
			menuSystemPushNewMenu(MENU_ZONE_LIST);
			return;
		}
	}

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

	if (freq_enter_idx==0)
	{
		if ((keys & KEY_STAR)!=0)
		{
			if (trxGetMode() == RADIO_MODE_ANALOG)
			{
				trxSetMode(RADIO_MODE_DIGITAL);
			}
			else
			{
				trxSetMode(RADIO_MODE_ANALOG);
			}
			currentChannelData->chMode = trxGetMode();
			menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		}
		else if ((keys & KEY_DOWN)!=0)
		{
			if (buttons & BUTTON_SK2 )
			{
				selectedFreq = VFO_SELECTED_FREQUENCY_INPUT_RX;
			}
			else
			{
				stepFrequency(FREQ_STEP * -1);
			}
			menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		}
		else if ((keys & KEY_UP)!=0)
		{
			if (buttons & BUTTON_SK2 )
			{
				selectedFreq = VFO_SELECTED_FREQUENCY_INPUT_TX;
			}
			else
			{
				stepFrequency(FREQ_STEP);
			}
			menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;

		}
		else if ((keys & KEY_RED)!=0)
		{
			menuSystemSetCurrentMenu(MENU_CHANNEL_MODE);
			return;
		}
		else if ((keys & KEY_RIGHT)!=0)
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
	}
	else
	{
		if ((keys & KEY_LEFT)!=0)
		{
			freq_enter_idx--;
			freq_enter_digits[freq_enter_idx]='-';
			menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		}
		else if ((keys & KEY_RED)!=0)
		{
			reset_freq_enter_digits();
    	    set_melody(melody_NACK_beep);
    		menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		}
	}
	if (freq_enter_idx<7)
	{
		char c='\0';
		if ((keys & KEY_0)!=0)
		{
			c='0';
		}
		else if ((keys & KEY_1)!=0)
		{
			c='1';
		}
		else if ((keys & KEY_2)!=0)
		{
			c='2';
		}
		else if ((keys & KEY_3)!=0)
		{
			c='3';
		}
		else if ((keys & KEY_4)!=0)
		{
			c='4';
		}
		else if ((keys & KEY_5)!=0)
		{
			c='5';
		}
		else if ((keys & KEY_6)!=0)
		{
			c='6';
		}
		else if ((keys & KEY_7)!=0)
		{
			c='7';
		}
		else if ((keys & KEY_8)!=0)
		{
			c='8';
		}
		else if ((keys & KEY_9)!=0)
		{
			c='9';
		}
		if (c!='\0')
		{
			freq_enter_digits[freq_enter_idx]=c;
			freq_enter_idx++;
			if (freq_enter_idx==7)
			{
				int tmp_frequency=read_freq_enter_digits();
				if (check_frequency(tmp_frequency))
				{
					update_frequency(tmp_frequency);
					reset_freq_enter_digits();
	        	    set_melody(melody_ACK_beep);
				}
				else
				{
	        	    set_melody(melody_ERROR_beep);
				}
			}
			menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		}
	}

	updateScreen();
}

static void stepFrequency(int increment)
{
int tmp_frequency;

	if (selectedFreq == VFO_SELECTED_FREQUENCY_INPUT_TX)
	{
		tmp_frequency  = currentChannelData->txFreq + increment;
	}
	else
	{
		tmp_frequency  = currentChannelData->rxFreq + increment;
	}
	if (check_frequency(tmp_frequency))
	{
		update_frequency(tmp_frequency);
	}
	else
	{
		set_melody(melody_ERROR_beep);
	}
}

