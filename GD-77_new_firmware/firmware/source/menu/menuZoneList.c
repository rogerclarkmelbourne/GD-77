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
#include <fw_codeplug.h>
#include "menu/menuSystem.h"
#include "fw_main.h"
#include "fw_settings.h"

static void updateScreen();
static void handleEvent(int buttons, int keys, int events);

int menuZoneList(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		gMenusEndIndex = codeplugZonesGetCount();
		gMenusCurrentItemIndex = nonVolatileSettings.currentZone;
		updateScreen();
	}
	else
	{
		if (events!=0 && keys!=0)
		{
			handleEvent(buttons, keys, events);
		}
	}
	return 0;
}

static void updateScreen()
{
	char nameBuf[17];
	const int headerOffset = 12;
	int rPos;
	struct_codeplugZone_t zoneBuf;
	UC1701_clearBuf();
	UC1701_printCentered(0, "Zones",UC1701_FONT_GD77_8x16);
	for(int i=-1; i <= 1 ;i++)
	{
		rPos= i + gMenusCurrentItemIndex;
		if (rPos >= gMenusEndIndex)
		{
			rPos=0;
		}
		if (rPos<0)
		{
			rPos = gMenusEndIndex+rPos;
		}
		if (rPos == gMenusCurrentItemIndex)
		{
			UC1701_fillRect(0,2+headerOffset + ((i+1)*16),128,16,false);
		}

		codeplugZoneGetDataForIndex(rPos,&zoneBuf);
		codeplugUtilConvertBufToString(zoneBuf.name,nameBuf,16);// need to convert to zero terminated string

		UC1701_printCore(5,(i+1)*16 + (headerOffset) ,(char *)nameBuf, UC1701_FONT_GD77_8x16,0,(rPos==gMenusCurrentItemIndex));
	}


	UC1701_render();
	displayLightTrigger();
}

static void handleEvent(int buttons, int keys, int events)
{
	if ((keys & KEY_DOWN)!=0)
	{
		gMenusCurrentItemIndex++;
		if (gMenusCurrentItemIndex >= gMenusEndIndex)
		{
			gMenusCurrentItemIndex=0;
		}
	}
	else if ((keys & KEY_UP)!=0)
	{
		gMenusCurrentItemIndex--;
		if (gMenusCurrentItemIndex < 0)
		{
			gMenusCurrentItemIndex=gMenusEndIndex-1;
		}
	}
	else if ((keys & KEY_GREEN)!=0)
	{
		nonVolatileSettings.currentZone = gMenusCurrentItemIndex;
		nonVolatileSettings.currentChannelIndexInZone = 0;// Since we are switching zones the channel index should be reset

		// Note. The Zone screen exits via the green button, back to whatever is selected as the bottom of the menu stack
		// This is normally the VFO or Channel screen.
		menuSystemPopAllAndDisplayRootMenu();

		return;
	}
	else if ((keys & KEY_RED)!=0)
	{
		menuSystemPopPreviousMenu();
		return;
	}
	updateScreen();
}
