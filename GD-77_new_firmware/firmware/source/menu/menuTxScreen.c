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

int menuTxScreen(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		gMenusCurrentItemIndex=180000;
		updateScreen();
	    GPIO_PinWrite(GPIO_LEDgreen, Pin_LEDgreen, 0);
	    GPIO_PinWrite(GPIO_LEDred, Pin_LEDred, 1);
	    trxSetFrequency(currentChannelData->txFreq);
	    txstartdelay=0;
	    txstopdelay=0;
		trxIsTransmitting=true;
	    trx_setTX();
	}
	else
	{
		if ((gMenusCurrentItemIndex%1000)==0)
		{
			updateScreen();
		}
		gMenusCurrentItemIndex--;
		if (gMenusCurrentItemIndex<0)
		{
			gMenusCurrentItemIndex=0;
		}

		handleEvent(buttons, keys, events);
	}
	return 0;
}

static void updateScreen()
{
	char buf[8];
	UC1701_clearBuf();
	sprintf(buf,"%d",gMenusCurrentItemIndex/1000);
	UC1701_printCentered(20, buf,UC1701_FONT_16x32);

	UC1701_render();
	displayLightOverrideTimeout(-1);
}


static void handleEvent(int buttons, int keys, int events)
{
	if ((buttons & BUTTON_PTT)==0)
	{
		if (txstopdelay>0)
		{
			trxIsTransmitting=false;
			txstopdelay--;
		}
		else
		{
			GPIO_PinWrite(GPIO_LEDred, Pin_LEDred, 0);
			trx_setRX();
			menuSystemPopPreviousMenu();
		}
	}
}

