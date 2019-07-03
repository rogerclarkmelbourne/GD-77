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
		menuTimer = 3000;// Increased so its easier to see what version of fw is being run
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
	UC1701_printCentered(12, "OpenGD77",3);
	UC1701_printCentered(32,(char *)FIRMWARE_VERSION_STRING,3);
	UC1701_printCentered(48,__DATE__,3);
	UC1701_render();
	displayLightTrigger();
}


static void handleEvent(int buttons, int keys, int events)
{
/*
if ((keys==KEY_1) || (keys==KEY_2) || (keys==KEY_3) || (keys==KEY_STAR))
{
	if (keys==KEY_1)
	{
		create_song(melody1);
	}
	else if (keys==KEY_2)
	{
		create_song(melody2);
	}
	else if (keys==KEY_3)
	{
		create_song(melody3);
	}
	else if (keys==KEY_STAR)
	{
		create_song(melody4);
	}
	set_melody(melody_generic);
	key_event=EVENT_KEY_NONE;
}
*/

	menuTimer--;
	if (menuTimer == 0)
	{
		menuSystemSetCurrentMenu(nonVolatileSettings.initialMenuNumber);
	}
}
