/*
 * Copyright (C)2019 Roger Clark. VK3KYY / G4KYF
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include "menu/menuSystem.h"
#include "menu/menuUtilityQSOData.h"
#include "fw_trx.h"
#include "fw_codeplug.h"
#include "fw_settings.h"

static void updateScreen();
static void handleEvent(int buttons, int keys, int events);
static int NUM_MENUS=3;
static struct_codeplugChannel_t tmpChannel;

int menuChannelDetails(int buttons, int keys, int events, bool isFirstRun)
{
	if (isFirstRun)
	{
		memcpy(&tmpChannel,currentChannelData,sizeof(struct_codeplugChannel_t));
		gMenusCurrentItemIndex=0;
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
	int mNum=0;
	char buf[17];
	UC1701_clearBuf();
	UC1701_printCentered(0, "Channel info",UC1701_FONT_GD77_8x16);

	// Can only display 3 of the options at a time menu at -1, 0 and +1
	for(int i=-1;i<=1;i++)
	{
		mNum = gMenusCurrentItemIndex+i;
		if (mNum<0)
		{
			mNum = NUM_MENUS + mNum;
		}
		if (mNum >= NUM_MENUS)
		{
			mNum = mNum - NUM_MENUS;
		}

		switch(mNum)
		{
		case 0:
			if (trxGetMode()==RADIO_MODE_ANALOG)
			{
				strcpy(buf,"Color Code:N/A");
			}
			else
			{
				sprintf(buf,"Color Code:%d",tmpChannel.rxColor);
			}
			break;
		case 1:
			if (trxGetMode()==RADIO_MODE_ANALOG)
			{
				strcpy(buf,"Timeslot:N/A");
			}
			else
			{
				sprintf(buf,"Timeslot:%d",((tmpChannel.flag2 & 0x40) >> 6) + 1);
			}
			break;
		case 2:
			if (trxGetMode()==RADIO_MODE_ANALOG)
			{
				if (tmpChannel.txTone==65535)
				{
					strcpy(buf,"CTCSS:None");
				}
				else
				{
					sprintf(buf,"CTCSS:%d.%dHz",tmpChannel.txTone/10 ,tmpChannel.txTone%10 );
				}
			}
			else
			{
				strcpy(buf,"CTCSS:N/A");
			}

			break;
		}

		if (gMenusCurrentItemIndex==mNum)
		{
			UC1701_fillRect(0,(i+2)*16,128,16,false);
		}

		UC1701_printCore(0,(i+2)*16,buf,UC1701_FONT_GD77_8x16,0,(gMenusCurrentItemIndex==mNum));
	}

	UC1701_render();
	displayLightTrigger();
}

static void handleEvent(int buttons, int keys, int events)
{
	if ((keys & KEY_DOWN)!=0 && gMenusEndIndex!=0)
	{
		gMenusCurrentItemIndex++;
		if (gMenusCurrentItemIndex>=NUM_MENUS)
		{
			gMenusCurrentItemIndex=0;
		}
	}
	else if ((keys & KEY_UP)!=0)
	{
		gMenusCurrentItemIndex--;
		if (gMenusCurrentItemIndex<0)
		{
			gMenusCurrentItemIndex=NUM_MENUS-1;
		}
	}
	else if ((keys & KEY_RIGHT)!=0)
	{
		switch(gMenusCurrentItemIndex)
		{
			case 0:
				if (tmpChannel.rxColor<15)
				{
					tmpChannel.rxColor++;
				}
				break;
			case 1:
				tmpChannel.flag2 |= 0x40;
				break;
			case 2:
				if (tmpChannel.txTone>0)
				{
					tmpChannel.txTone--;
				}
				break;
		}
	}
	else if ((keys & KEY_LEFT)!=0)
	{
		switch(gMenusCurrentItemIndex)
		{
			case 0:
				if (tmpChannel.rxColor>0)
				{
					tmpChannel.rxColor--;
				}
				break;
			case 1:
				tmpChannel.flag2 &= 0xBF;
				break;
			case 2:
				if (tmpChannel.txTone>0)
				{
					tmpChannel.txTone--;
				}
				break;
		}
	}
	else if ((keys & KEY_GREEN)!=0)
	{
		memcpy(currentChannelData,&tmpChannel,sizeof(struct_codeplugChannel_t));
		menuSystemPopPreviousMenu();
		return;
	}
	else if ((keys & KEY_RED)!=0)
	{
		menuSystemPopPreviousMenu();
		return;
	}
	updateScreen();
}
