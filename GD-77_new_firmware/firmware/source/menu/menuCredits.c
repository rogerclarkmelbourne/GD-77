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
static void scrollDownOneLine();

#define CREDIT_TEXT_LENGTH 33
static const int NUM_LINES_PER_SCREEN = 7;
static const char creditTexts[][CREDIT_TEXT_LENGTH] = {"Conceived & developed","by","Kai DG4KLU","with help from","Roger VK3KYY"};
static int currentDisplayIndex=0;

int menuCredits(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		gMenusCurrentItemIndex=5000;
		menuTimer = 3000;
		updateScreen();
	}
	else
	{
		if (events!=0 && keys!=0)
		{
			handleEvent(buttons, keys, events);
		}
/*
 * Uncomment to enable auto scrolling
		if ((gMenusCurrentItemIndex--)==0)
		{
			scrollDownOneLine();
			gMenusCurrentItemIndex=1000;
		}
*/
	}
	return 0;
}

static void updateScreen()
{
	int numCredits = sizeof(creditTexts)/CREDIT_TEXT_LENGTH;
	UC1701_clearBuf();

	UC1701_printCentered(0,	"OpenGD77",UC1701_FONT_GD77_8x16);

	for(int i=0;i<6;i++)
	{
		if ((i+currentDisplayIndex) < numCredits)
		{
			UC1701_printCentered(i*8 + 16,(char *)creditTexts[i+currentDisplayIndex],0);
		}
	}
	UC1701_render();
	displayLightTrigger();
}

static void scrollDownOneLine()
{
	int numCredits = sizeof(creditTexts)/CREDIT_TEXT_LENGTH;
	if (currentDisplayIndex < (numCredits - NUM_LINES_PER_SCREEN) )
	{
		currentDisplayIndex++;
	}
	updateScreen();
}

static void handleEvent(int buttons, int keys, int events)
{

	if ((keys & KEY_RED)!=0)
	{
		menuSystemPopPreviousMenu();
		return;
	}
	else if ((keys & KEY_DOWN)!=0)
	{
		scrollDownOneLine();
	}
	else if ((keys & KEY_UP)!=0)
	{
		if (currentDisplayIndex>0)
		{
			currentDisplayIndex--;
		}
		updateScreen();
	}
}
