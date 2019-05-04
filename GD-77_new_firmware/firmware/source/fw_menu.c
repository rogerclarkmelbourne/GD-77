/*
 * Copyright (C)2019 Kai Ludwig, DG4KLU
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

#include "fw_menu.h"

typedef struct menu_item menu_item_t;
struct menu_item
{
	char* menu_item_text;
	menu_item_t* sub_menu;
};

menu_item_t subsub_menu1[] = {
		{ "SubSub 1/1", NULL },
		{ "SubSub 1/2", NULL },
		{ NULL, NULL }
};

menu_item_t subsub_menu2[] = {
		{ "SubSub 2/1", NULL },
		{ "SubSub 2/2", NULL },
		{ NULL, NULL }
};

menu_item_t sub_menu1[] = {
		{ "Sub 1/1", NULL },
		{ "Sub 1/2", NULL },
		{ "To SubSub 1", subsub_menu1 },
		{ "To SubSub 2", subsub_menu2 },
		{ NULL, NULL }
};

menu_item_t sub_menu2[] = {
		{ "Sub 2/1", NULL },
		{ "Sub 2/2", NULL },
		{ NULL, NULL }
};

menu_item_t top_menu[] = {
		{ "Top 1", NULL },
		{ "Top 2", NULL },
		{ "To Sub 1", sub_menu1 },
		{ "To Sub 2", sub_menu2 },
		{ NULL, NULL }
};

typedef struct menu_level menu_level_t;
struct menu_level
{
	menu_item_t* current_menu;
	int current_menu_item;
};

#define MAX_MENU_LEVELS 4
menu_level_t menu_levels[MAX_MENU_LEVELS];
int current_menu_level;

void update_menu()
{
	UC1701_clear();
	UC1701_printCentered(2, "MENU MODE");
	UC1701_printCentered(4, menu_levels[current_menu_level].current_menu[menu_levels[current_menu_level].current_menu_item].menu_item_text);
	Display_light_Touched = true;
}

void reset_menu()
{
	current_menu_level = -1;
}

void tick_menu()
{
	if ((current_menu_level==-1) && ((buttons & BUTTON_SK2)!=0) && (button_event==EVENT_BUTTON_CHANGE)) // Enter Menu
	{
		reset_splashscreen();
		current_menu_level = 0;
		menu_levels[current_menu_level].current_menu = top_menu;
		menu_levels[current_menu_level].current_menu_item = 0;
		update_menu();
	}
	else if ((current_menu_level>=0) && ((buttons & BUTTON_SK2)!=0) && (button_event==EVENT_BUTTON_CHANGE)) // Exit Menu
	{
		reset_menu();
		update_screen();
	}
	else if (current_menu_level>=0)
	{
		if  (key_event==EVENT_KEY_CHANGE)
		{
			if ((keys & KEY_RIGHT)!=0) // Menu item right
			{
				menu_levels[current_menu_level].current_menu_item++;
				if ( menu_levels[current_menu_level].current_menu[menu_levels[current_menu_level].current_menu_item].menu_item_text == NULL)
				{
					menu_levels[current_menu_level].current_menu_item = 0;
				}
			}
			else if ((keys & KEY_LEFT)!=0) // Menu item left
			{
				menu_levels[current_menu_level].current_menu_item--;
				if ( menu_levels[current_menu_level].current_menu_item < 0)
				{
					int tmp_current_menu_item = 0;
					while (menu_levels[current_menu_level].current_menu[tmp_current_menu_item].menu_item_text != NULL)
					{
						tmp_current_menu_item++;
					}
					menu_levels[current_menu_level].current_menu_item = tmp_current_menu_item - 1;
				}
			}
			else if ((keys & KEY_DOWN)!=0) // Menu level down
			{
				if (current_menu_level<MAX_MENU_LEVELS-1)
				{
					menu_item_t* tmp_sub_menu = menu_levels[current_menu_level].current_menu[menu_levels[current_menu_level].current_menu_item].sub_menu;
					if (tmp_sub_menu!=NULL)
					{
						current_menu_level++;
						menu_levels[current_menu_level].current_menu = tmp_sub_menu;
						menu_levels[current_menu_level].current_menu_item = 0;
					}
				}
			}
			else if ((keys & KEY_UP)!=0) // Menu level up
			{
				if (current_menu_level>0)
				{
					current_menu_level--;
				}
			}

			update_menu();
		}
	}
}
