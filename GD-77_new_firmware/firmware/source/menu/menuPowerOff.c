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


int menuPowerOff(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuTimer = 1200;// Not sure why its this value. But never mind ;-)
		updateScreen();
	}
	else
	{
		handleEvent(buttons, keys, events);
	}
	return 0;
}

static void updateScreen()
{
	UC1701_clearBuf();
	UC1701_printCentered(12, "Power off ..",UC1701_FONT_GD77_8x16);
	UC1701_printCentered(32, "73",UC1701_FONT_GD77_8x16);
	UC1701_render();
	displayLightTrigger();
}

static void handleEvent(int buttons, int keys, int events)
{
	if ((GPIO_PinRead(GPIO_Power_Switch, Pin_Power_Switch)==0) && (battery_voltage>CUTOFF_VOLTAGE_LOWER_HYST))
	{
		// I think this is to handle if the power button is turned back on during shutdown
		menuSystemPopPreviousMenu();
		return;
	}

	menuTimer--;

	if (menuTimer == 0)
	{
		// This turns the power off to the CPU.
		GPIO_PinWrite(GPIO_Keep_Power_On, Pin_Keep_Power_On, 0);
	}
}
