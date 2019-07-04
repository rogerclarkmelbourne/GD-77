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

int menuDisplayLightTimer=-1;
int menuTimer;
menuItemNew_t *gMenuCurrentMenuList;

menuControlDataStruct_t menuControlData = { .stackPosition = 0, .stack = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};


int menuVFOMode(int buttons, int keys, int events, bool isFirstRun);
int menuChannelMode(int buttons, int keys, int events, bool isFirstRun);
int menuZoneList(int buttons, int keys, int events, bool isFirstRun);
int menuDisplayMenuList(int buttons, int keys, int events, bool isFirstRun);
int menuBattery(int buttons, int keys, int events, bool isFirstRun);
int menuSplashScreen(int buttons, int keys, int events, bool isFirstRun);
int menuPowerOff(int buttons, int keys, int events, bool isFirstRun);
int menuFirmwareInfoScreen(int buttons, int keys, int events, bool isFirstRun);
int menuNumericalEntry(int buttons, int keys, int events, bool isFirstRun);
int menuTxScreen(int buttons, int keys, int events, bool isFirstRun);
int menuRSSIScreen(int buttons, int keys, int events, bool isFirstRun);
int menuLastHeard(int buttons, int keys, int events, bool isFirstRun);
int menuDebug(int buttons, int keys, int events, bool isFirstRun);
int menuDisplayOptions(int buttons, int keys, int events, bool isFirstRun);
int menuCredits(int buttons, int keys, int events, bool isFirstRun);


/*
 * ---------------------- IMPORTANT ----------------------------
 *
 * The menuFunctions array and the menusData array.....
 *
 * MUST match the enum MENU_SCREENS in menuSystem.h
 *
 * ---------------------- IMPORTANT ----------------------------
 */
const menuItemNew_t * menusData[] = { 	NULL,// splash
										NULL,// power off
										NULL,// vfo mode
										NULL,// channel mode
										menuDataMainMenu,
										menuDataContact,
										NULL,// zone
										NULL,// Battery
										NULL,// Firmwareinfo
										NULL,// Numerical entry
										NULL,// Tx
										NULL,// RSSI
										NULL,// LastHeard
										NULL,// Debug
								};

const MenuFunctionPointer_t menuFunctions[] = { menuSplashScreen,
												menuPowerOff,
												menuVFOMode,
												menuChannelMode,
												menuDisplayMenuList,
												menuDisplayMenuList,
												menuZoneList,
												menuBattery,
												menuFirmwareInfoScreen,
												menuNumericalEntry,
												menuTxScreen,
												menuRSSIScreen,
												menuLastHeard,
												menuDebug,
												menuDisplayOptions,
												menuCredits};

void menuSystemPushNewMenu(int menuNumber)
{
	menuControlData.stackPosition++;
	menuControlData.stack[menuControlData.stackPosition] = menuNumber;
	menuFunctions[menuControlData.stack[menuControlData.stackPosition]](0,0,0,true);
}
void menuSystemPopPreviousMenu()
{
	menuControlData.stackPosition--;
	menuFunctions[menuControlData.stack[menuControlData.stackPosition]](0,0,0,true);
}
void menuSystemPopAllAndDisplayRootMenu()
{
	menuControlData.stackPosition=0;
	menuFunctions[menuControlData.stack[menuControlData.stackPosition]](0,0,0,true);
}
void menuSystemSetCurrentMenu(int menuNumber)
{
	menuControlData.stack[menuControlData.stackPosition]  = menuNumber;
	menuFunctions[menuControlData.stack[menuControlData.stackPosition]](0,0,0,true);
}
int menuSystemGetCurrentMenuNumber()
{
	return menuControlData.stack[menuControlData.stackPosition];
}

void menuSystemCallCurrentMenuTick(int buttons, int keys, int events)
{
	menuFunctions[menuControlData.stack[menuControlData.stackPosition]](buttons,keys,events,false);
}

void displayLightTrigger()
{
	menuDisplayLightTimer = nonVolatileSettings.backLightTimeout * 1000;
	fw_displayEnableBacklight(true);
}

// use -1 to force LED on all the time
void displayLightOverrideTimeout(int timeout)
{
	menuDisplayLightTimer = timeout;
	fw_displayEnableBacklight(true);
}

const int MENU_EVENT_SAVE_SETTINGS = -1;
int gMenusCurrentItemIndex; // each menu can re-use this var to hold the position in their display list. To save wasted memory if they each had their own variable
int gMenusStartIndex;// as above
int gMenusEndIndex;// as above


void menuInitMenuSystem()
{
	menuDisplayLightTimer = -1;
	menuControlData.stack[menuControlData.stackPosition]  = MENU_SPLASH_SCREEN;// set the very first screen as the splash screen
	menuFunctions[menuControlData.stack[menuControlData.stackPosition]](0,0,0,true);// Init and display this screen
}

const char menuStringTable[32][16] = { "",//0
                                         "Menu",//1
                                         "Contact",//2
                                         "Message",//3
                                         "Call Logs",//4
                                         "Set",//5
                                         "Zone",//6
                                         "New Contact",//7
                                         "Manual Dial",//8
                                         "InBox",//9
                                         "New Message",//10
                                         "Manual Dial",//11
                                         "OutBox",//12
                                         "Draft",//13
                                         "Quick test",//14
										 "Battery",//15
										 "Firmware info",//16
										 "RSSI",//17
										 "Last heard",//18
										 "Debug",//19
										 "Display options",//20
										 "Credits",//21
};

const menuItemNew_t menuDataMainMenu[] = {
	{8,8},// number of enties i
	{ 21, MENU_CREDITS },
	{ 6, MENU_ZONE_LIST },
	{ 17, MENU_RSSI_SCREEN },
	{ 15, MENU_BATTERY },
	{ 18, MENU_LAST_HEARD },
	{ 16, MENU_FIRMWARE_INFO },
	{ 19, MENU_DEBUG },
	{ 20, MENU_DISPLAY},
};
const menuItemNew_t menuDataContact[] = {
	{ 3, 3 } ,// length
	{ 2, -1 },// Contact
	{ 7 , -1 },// New Contact
	{ 8, -1 } // Manual Dial
};

const menuItemNew_t menuDataContactContact [] = {
	{ 6,6 },// length
	{ 9, -1 },// InBox
	{ 10, -1 },// New Message
	{ 11, -1 },// Manual Dial
	{ 12, -1 },// OutBox
	{ 13, -1 },// Draft
	{ 14, -1 }// Quick Test
};



int menuDisplayList(int buttons, int keys, int events, bool isFirstRun)
{
	return 0;
}

