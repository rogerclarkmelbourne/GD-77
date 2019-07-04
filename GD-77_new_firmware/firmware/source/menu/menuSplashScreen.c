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

static void updateScreen();
static void handleEvent(int buttons, int keys, int events);

int menuSplashScreen(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		menuTimer = 3000;
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
	char line1[16];
	char line2[16];

	codeplugGetBootItemTexts(line1,line2);
	UC1701_clearBuf();
	UC1701_printCentered(10, "OpenGD77",UC1701_FONT_GD77_8x16);
	UC1701_printCentered(28, line1,UC1701_FONT_GD77_8x16);
	UC1701_printCentered(42, line2,UC1701_FONT_GD77_8x16);
	UC1701_render();
	displayLightTrigger();
}

static void handleEvent(int buttons, int keys, int events)
{
	menuTimer--;
	if (menuTimer == 0)
	{
		menuSystemSetCurrentMenu(nonVolatileSettings.initialMenuNumber);
	}
}
