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



static char freq_enter_digits[7] = { '-', '-', '-', '-', '-', '-', '-' };
static int freq_enter_idx = 0;

static int FREQ_STEP =	125;// will load from settings

// internal prototypes
static void updateScreen();
static void handleEvent(int buttons, int keys, int events);

static void reset_freq_enter_digits();
static int read_freq_enter_digits();
static bool check_frequency(int tmp_frequency);
static void update_frequency(int tmp_frequency);
bool menuIsDisplayingQSOData = false;// HACK ALERT. DO NOT DELETE THIS LINE. BECAUSE THE DMR AUDIO WILL STOP WOKRING

// public interface
int menuVFOMode(int buttons, int keys, int events, bool isFirstRun)
{
	menuIsDisplayingQSOData=true;// HACK ALERT. DO NOT DELETE THIS LINE. BECAUSE THE DMR AUDIO WILL STOP WOKRING
	if (isFirstRun)
	{
		nonVolatileSettings.initialMenuNumber=MENU_VFO_MODE;
		trxSetFrequency(nonVolatileSettings.vfoFrequenciesArray[nonVolatileSettings.currentVFOIndex]);
		trxSetMode(nonVolatileSettings.vfoTrxMode);
		trxSetPower(nonVolatileSettings.txPower);
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
	char buffer[32];
	UC1701_clearBuf();
	menuUtilityRenderHeader();

	switch(menuDisplayQSODataState)
	{
		case QSO_DISPLAY_DEFAULT_SCREEN:
			sprintf(buffer,"TG %d",trxTalkGroup);

			UC1701_printCentered(20,buffer,UC1701_FONT_GD77_8x16);

			if (freq_enter_idx==0)
			{
				int val_before_dp = nonVolatileSettings.vfoFrequenciesArray[nonVolatileSettings.currentVFOIndex]/10000;
				int val_after_dp = nonVolatileSettings.vfoFrequenciesArray[nonVolatileSettings.currentVFOIndex] - val_before_dp*10000;
				sprintf(buffer,"#%d %d.%04d MHz", nonVolatileSettings.currentVFOIndex+1, val_before_dp, val_after_dp);
			}
			else
			{
				sprintf(buffer,"#%d %c%c%c.%c%c%c%c MHz", nonVolatileSettings.currentVFOIndex+1, freq_enter_digits[0], freq_enter_digits[1], freq_enter_digits[2], freq_enter_digits[3], freq_enter_digits[4], freq_enter_digits[5], freq_enter_digits[6] );
			}
			UC1701_printCentered(40, buffer,UC1701_FONT_GD77_8x16);
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
	nonVolatileSettings.vfoFrequenciesArray[nonVolatileSettings.currentVFOIndex]=frequency;
	trxSetFrequency(frequency);
}

static void handleEvent(int buttons, int keys, int events)
{
	if (events &0x02)
	{
		if (buttons & 0x08)
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
		if ((keys & KEY_RIGHT)!=0)
		{
			nonVolatileSettings.currentVFOIndex++;
			if (nonVolatileSettings.currentVFOIndex>VFO_COUNT-1)
			{
				nonVolatileSettings.currentVFOIndex=0;
			}
			trxSetFrequency(nonVolatileSettings.vfoFrequenciesArray[nonVolatileSettings.currentVFOIndex]);
			menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		}
		else if ((keys & KEY_LEFT)!=0)
		{
			nonVolatileSettings.currentVFOIndex--;
			if (nonVolatileSettings.currentVFOIndex<0)
			{
				nonVolatileSettings.currentVFOIndex=VFO_COUNT-1;
			}
			trxSetFrequency(nonVolatileSettings.vfoFrequenciesArray[nonVolatileSettings.currentVFOIndex]);
			menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
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
			nonVolatileSettings.vfoTrxMode=trxGetMode();
			menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		}
		else if ((keys & KEY_DOWN)!=0)
		{
			int tmp_frequency=nonVolatileSettings.vfoFrequenciesArray[nonVolatileSettings.currentVFOIndex]-FREQ_STEP;
			if (check_frequency(tmp_frequency))
			{
				update_frequency(tmp_frequency);
			}
			else
			{
        	    set_melody(melody_ERROR_beep);
			}
			menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		}
		else if ((keys & KEY_UP)!=0)
		{
			int tmp_frequency=nonVolatileSettings.vfoFrequenciesArray[nonVolatileSettings.currentVFOIndex]+FREQ_STEP;
			if (check_frequency(tmp_frequency))
			{
				update_frequency(tmp_frequency);
			}
			else
			{
        	    set_melody(melody_ERROR_beep);
			}
			menuDisplayQSODataState = QSO_DISPLAY_DEFAULT_SCREEN;
		}
		else if ((keys & KEY_RED)!=0)
		{
			menuSystemSetCurrentMenu(MENU_CHANNEL_MODE);
			return;
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

