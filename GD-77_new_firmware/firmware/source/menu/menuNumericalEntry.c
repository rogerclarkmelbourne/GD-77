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
#include "fw_settings.h"

static char digits[9];
static void updateScreen();
static void handleEvent(int buttons, int keys, int events);

static const char *menuName[2]={"TG entry","Manual dial"};

// public interface
int menuNumericalEntry(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		gMenusCurrentItemIndex=0;
		digits[0]=0x00;
		updateScreen();
	}
	else
	{
		if (events==0)
		{
			//updateScreen();
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
	UC1701_clearBuf();

	UC1701_printCentered(8, (char *)menuName[gMenusCurrentItemIndex],UC1701_FONT_GD77_8x16);

	UC1701_printCentered(32, (char *)digits,UC1701_FONT_GD77_8x16);
	displayLightTrigger();

	UC1701_render();
}

static void handleEvent(int buttons, int keys, int events)
{
	if ((keys & KEY_RED)!=0)
	{
		menuSystemPopPreviousMenu();
		return;
	}
	else if ((keys & KEY_GREEN)!=0)
	{
		trxTalkGroup = atoi(digits);
		settingsIsTgOverride=true;
		menuSystemPopAllAndDisplayRootMenu();
	}
	else if ((keys & KEY_HASH)!=0)
	{
		gMenusCurrentItemIndex = 1 - gMenusCurrentItemIndex;
		updateScreen();
	}

	if (strlen(digits)<7)
	{
		char c[2]={0,0};
		if ((keys & KEY_0)!=0)
		{
			c[0]='0';
		}
		else if ((keys & KEY_1)!=0)
		{
			c[0]='1';
		}
		else if ((keys & KEY_2)!=0)
		{
			c[0]='2';
		}
		else if ((keys & KEY_3)!=0)
		{
			c[0]='3';
		}
		else if ((keys & KEY_4)!=0)
		{
			c[0]='4';
		}
		else if ((keys & KEY_5)!=0)
		{
			c[0]='5';
		}
		else if ((keys & KEY_6)!=0)
		{
			c[0]='6';
		}
		else if ((keys & KEY_7)!=0)
		{
			c[0]='7';
		}
		else if ((keys & KEY_8)!=0)
		{
			c[0]='8';
		}
		else if ((keys & KEY_9)!=0)
		{
			c[0]='9';
		}
		else if ((keys & KEY_LEFT)!=0 && strlen(digits)>0)
		{
			digits[strlen(digits)-1]=0;
			updateScreen();
		}
		if (c[0]!=0)
		{
			strcat(digits,c);
			updateScreen();
		}
	}
}
