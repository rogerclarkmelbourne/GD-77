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
#ifndef _FW_MENUSYSTEM_H_
#define _FW_MENUSYSTEM_H_
#include "fw_main.h"

extern int menuDisplayLightTimer;
extern int menuTimer;

typedef int (*MenuFunctionPointer_t)(int,int,int,bool); // Typedef for menu function pointers.  Functions are passed the key, the button and the event data. Event can be a Key or a button or both. Last arg is for when the function is only called to initialise and display its screen.
typedef struct menuControlDataStruct
{
	int currentMenuNumber;
	int stackPosition;
	int stack[16];
} menuControlDataStruct_t;

typedef struct menuItemNew
{
       int stringNumber;
       int menuNum;
} menuItemNew_t;

void menuInitMenuSystem();
void displayLightTrigger();
void displayLightOverrideTimeout(int timeout);
void menuSystemPushNewMenu(int menuNumber);

void menuSystemSetCurrentMenu(int menuNumber);
int menuSystemGetCurrentMenuNumber();

void menuSystemPopPreviousMenu();
void menuSystemPopAllAndDisplayRootMenu();
void menuSystemPopAllAndDisplaySpecificRootMenu(int newRootMenu);

void menuSystemCallCurrentMenuTick(int buttons, int keys, int events);

/*
 * ---------------------- IMPORTANT ----------------------------
 *
 * These enums must match the menuFunctions array in menuSystem.c
 *
 * ---------------------- IMPORTANT ----------------------------
 */
enum MENU_SCREENS { MENU_SPLASH_SCREEN=0,
					MENU_POWER_OFF,
					MENU_VFO_MODE,
					MENU_CHANNEL_MODE,
					MENU_MAIN_MENU,
					MENU_CONTACTS_MENU,
					MENU_ZONE_LIST,
					MENU_BATTERY,
					MENU_FIRMWARE_INFO,
					MENU_NUMERICAL_ENTRY,
					MENU_TX_SCREEN,
					MENU_RSSI_SCREEN,
					MENU_LAST_HEARD,
					MENU_DEBUG,
					MENU_DISPLAY,
					MENU_CREDITS};

extern int gMenusCurrentItemIndex;
extern int gMenusStartIndex;
extern int gMenusEndIndex;
extern menuItemNew_t *gMenuCurrentMenuList;

extern const char menuStringTable[32][16];

extern const menuItemNew_t menuDataMainMenu[];
extern const menuItemNew_t menuDataContact[];
extern const menuItemNew_t menuDataContactContact [];
extern const menuItemNew_t * menusData[];

#endif
