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

static void updateScreen();
static void handleEvent(int buttons, int keys, int events);

int menuLastHeard(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		gMenusCurrentItemIndex=0;
		gMenusStartIndex = LinkHead->id;// reuse this global to store the ID of the first item in the list
		gMenusEndIndex=0;
		updateScreen();
	}
	else
	{
		// do live update by checking if the item at the top of the list has changed
		if (gMenusStartIndex != LinkHead->id)
		{
			gMenusStartIndex = LinkHead->id;
			gMenusCurrentItemIndex=0;
			gMenusEndIndex=0;
			updateScreen();
		}
		if (events!=0 && keys!=0)
		{
			handleEvent(buttons, keys, events);
		}
	}
	return 0;
}

static void updateScreen()
{
	dmrIdDataStruct_t foundRecord;
	int numDisplayed=0;
	LinkItem_t *item = LinkHead;


	UC1701_clearBuf();
	UC1701_printCentered(0, "Last heard",UC1701_FONT_GD77_8x16);

	// skip over the first gMenusCurrentItemIndex in the listing
	for(int i=0;i<gMenusCurrentItemIndex;i++)
	{
		item=item->next;
	}
	while((item != NULL) && item->id != 0)
	{
		memset(&foundRecord,0,sizeof(dmrIdDataStruct_t));
		dmrIDLookup(item->id,&foundRecord);
		{
			UC1701_printCentered(16+(numDisplayed*16), foundRecord.text,UC1701_FONT_GD77_8x16);
			numDisplayed++;
		}
		item=item->next;
		if (numDisplayed>3)
		{
			if (item!=NULL && item->id != 0)
			{
				gMenusEndIndex=0x01;
			}
			else
			{
				gMenusEndIndex=0;
			}
			break;
		}
	}

	UC1701_render();
	displayLightTrigger();
}


static void handleEvent(int buttons, int keys, int events)
{

	if ((keys & KEY_DOWN)!=0 && gMenusEndIndex!=0)
	{
		gMenusCurrentItemIndex++;
	}
	else if ((keys & KEY_UP)!=0)
	{
		gMenusCurrentItemIndex--;
		if (gMenusCurrentItemIndex<0)
		{
			gMenusCurrentItemIndex=0;
		}
	}
	else if ((keys & KEY_RED)!=0)
	{
		menuSystemPopPreviousMenu();
		return;
	}
	updateScreen();
}
