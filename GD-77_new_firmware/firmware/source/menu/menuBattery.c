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

static void updateScreen();
static void handleEvent(int buttons, int keys, int events);

int menuBattery(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
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
	const int BATTERY_MIN_VOLTAGE = 7;
	const float BATTERY_MAX_VOLTAGE = 8.4;
	char buffer[8];

	UC1701_clearBuf();
	UC1701_printCentered(0, "Battery",UC1701_FONT_GD77_8x16);

	int val1 = battery_voltage/10;
	int val2 = battery_voltage - (val1 * 10);

	sprintf(buffer,"%d.%dV", val1,val2);
	UC1701_printAt(24,16, buffer,UC1701_FONT_16x32);
	uint32_t h = (uint32_t)(((battery_voltage - BATTERY_MIN_VOLTAGE) * 50)/(BATTERY_MAX_VOLTAGE-BATTERY_MIN_VOLTAGE));

	if (h>50)
	{
		h=50;
	}
	// draw frame
	UC1701_fillRect(100,10,24,52,false);
	UC1701_fillRect(101,11,22,50,true);

	UC1701_fillRect(101, 11 + 50 - h ,22,h,false);// fill battery

	for(int y=56; y > 11;y-=5)
	{
		UC1701_fillRect(101,y,22,1,true);
	}

	UC1701_render();
	displayLightTrigger();
}


static void handleEvent(int buttons, int keys, int events)
{
	if ((keys & KEY_RED)!=0)
	{
		menuSystemPopPreviousMenu();
		return;
	}
}
