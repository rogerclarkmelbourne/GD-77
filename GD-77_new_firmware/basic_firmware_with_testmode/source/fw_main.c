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

#include "virtual_com.h"

#include "fw_main.h"

#include "UC1701.h"

TaskHandle_t fwMainTaskHandle;

void fw_init()
{
	xTaskCreate(fw_main_task,                        /* pointer to the task */
				"fw main task",                      /* task name for kernel awareness debugging */
				5000L / sizeof(portSTACK_TYPE),      /* task stack size */
				NULL,                      			 /* optional task startup argument */
				5U,                                  /* initial priority */
				fwMainTaskHandle					 /* optional task handle to create */
				);

    vTaskStartScheduler();
}

int Display_light_Timer = 0;
bool Display_light_Touched = false;
bool Show_SplashScreen = false;
int SplashScreen_Timer = 0;
bool Shutdown = false;
int Shutdown_Timer = 0;

void show_splashscreen()
{
	UC1701_clear();
	UC1701_printCentered(1, "Experimental");
	UC1701_printCentered(2, "firmware V0.0.0");
	UC1701_printCentered(4, "by");
	UC1701_printCentered(6, "DG4KLU");
	Display_light_Touched = true;
}

void show_running()
{
	UC1701_clear();
	UC1701_printCentered(4, "RUNNING");
	UC1701_printCentered(6, "(with testmode)");
	Display_light_Touched = true;
}

void show_poweroff()
{
	UC1701_clear();
	UC1701_printCentered(2, "Power off ...");
	UC1701_printCentered(4, "73 de DG4KLU");
	Display_light_Touched = true;
}

void reset_splashscreen()
{
	Show_SplashScreen=false;
	SplashScreen_Timer=0;
}

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

int testmode;
int testmode_ptt_timer;

uint32_t receive_data1;
uint32_t receive_data2;
uint32_t send_data1;
uint32_t send_data2;
uint32_t remotetest_timer;

void show_testmode()
{
	UC1701_clear();
	UC1701_printCentered(2, "TEST MODE");
	UC1701_printCentered(5, "Hold PTT: disp fill");
	UC1701_printCentered(6, "Hold SK1: exit     ");
	Display_light_Touched = true;
}

void reset_testmode()
{
	testmode=0;
	testmode_ptt_timer=0;
	GPIO_PinWrite(GPIO_LEDgreen, Pin_LEDgreen, 0);
	GPIO_PinWrite(GPIO_LEDred, Pin_LEDred, 0);

	receive_data1 = 0;
	receive_data2 = 0;
	send_data1 = 0;
	send_data2 = 0;
	remotetest_timer = 0;
}

void fw_main_task()
{
    USB_DeviceApplicationInit();

	fw_init_common();
	fw_init_buttons();
	fw_init_LEDs();
	fw_init_keyboard();
	fw_init_display();

    // Small startup delay after initialization to stabilize system
    vTaskDelay(portTICK_PERIOD_MS * 100);

	Show_SplashScreen = true;

	reset_menu();
	reset_testmode();

    while (1U)
    {
    	// Read button state and event
    	uint32_t buttons;
		int button_event;
    	fw_check_button_event(&buttons, &button_event);

    	// Read keyboard state and event
    	uint32_t keys;
    	int key_event;
    	fw_check_key_event(&keys, &key_event);

    	if (!Shutdown)
    	{
        	if (testmode>0)
        	{
        		if ((receive_data1 & 0x80000000) != 0)
        		{
        			if ((receive_data1 & 0x00000001) != 0)
        			{
        			    GPIO_PinWrite(GPIO_LEDgreen, Pin_LEDgreen, 1);
        			}
        			else
        			{
        			    GPIO_PinWrite(GPIO_LEDgreen, Pin_LEDgreen, 0);
        			}
        			if ((receive_data1 & 0x00000002) != 0)
        			{
        			    GPIO_PinWrite(GPIO_LEDred, Pin_LEDred, 1);
        			}
        			else
        			{
        			    GPIO_PinWrite(GPIO_LEDred, Pin_LEDred, 0);
        			}
        			receive_data1 = 0;
        			remotetest_timer = 2000;
        		}
        		if ((receive_data2 & 0x80000000) != 0)
        		{
        			if ((receive_data2 & 0x00000001) != 0)
        			{
        				UC1701_printCentered(1,"DISPTEST DISPTEST");
        			}
        			else
        			{
        				UC1701_printCentered(1,"                 ");
        			}
        			if ((receive_data2 & 0x00000002) != 0)
        			{
        				GPIO_PinWrite(GPIO_Display_Light, Pin_Display_Light, 1);
        			}
        			else
        			{
        				GPIO_PinWrite(GPIO_Display_Light, Pin_Display_Light, 0);
        			}
        			Display_light_Timer = 0;
        			Display_light_Touched = false;
        			receive_data2 = 0;
        			remotetest_timer = 2000;
        		}
        		send_data1 = buttons;
        		send_data2 = keys;

        		if (remotetest_timer>0)
        		{
        			remotetest_timer--;
        			if (remotetest_timer==0)
        			{
        			    GPIO_PinWrite(GPIO_LEDgreen, Pin_LEDgreen, 0);
        			    GPIO_PinWrite(GPIO_LEDred, Pin_LEDred, 0);
        				UC1701_printCentered(1,"                 ");
        				GPIO_PinWrite(GPIO_Display_Light, Pin_Display_Light, 0);
        			}
        		}

            	if ((buttons & BUTTON_PTT)!=0)
            	{
            		if (testmode_ptt_timer<2000)
            		{
            			testmode_ptt_timer++;
            			if (testmode_ptt_timer==2000)
            			{
            			    GPIO_PinWrite(GPIO_LEDgreen, Pin_LEDgreen, 0);
            			    GPIO_PinWrite(GPIO_LEDred, Pin_LEDred, 0);
            				UC1701_clear();
            				for (int i=0; i<8; i++)
            				{
                				UC1701_printAt(1, i, "XXXXXXXXXXXXXXXXXXXXX");
            				}
            			}
            		}
        			Display_light_Touched = true;
            	}
            	else
            	{
        			if (testmode_ptt_timer==2000)
        			{
                		show_testmode();
        			}
        			testmode_ptt_timer = 0;
            	}

        		if (testmode_ptt_timer<2000)
        		{
            		if (button_event)
            		{
        				UC1701_printCentered(3, "                    ");
            			if ((buttons & BUTTON_PTT)!=0)
            			{
            				UC1701_printCentered(3, "Button: PTT");
            			}
            			else if ((buttons & BUTTON_SK1)!=0)
            			{
            				UC1701_printCentered(3, "Button: SK1");
            			}
            			else if ((buttons & BUTTON_SK2)!=0)
            			{
            				UC1701_printCentered(3, "Button: SK2");
            			}
            			else if ((buttons & BUTTON_ORANGE)!=0)
            			{
            				UC1701_printCentered(3, "Button: ORANGE");
            			}
            			Display_light_Touched = true;
            		}

            		if (key_event)
            		{
            			if ((keys & KEY_GREEN)!=0)
            			{
            			    GPIO_PinWrite(GPIO_LEDgreen, Pin_LEDgreen, 1);
            			}
            			else
            			{
            			    GPIO_PinWrite(GPIO_LEDgreen, Pin_LEDgreen, 0);
            			}
            			if ((keys & KEY_RED)!=0)
            			{
            			    GPIO_PinWrite(GPIO_LEDred, Pin_LEDred, 1);
            			}
            			else
            			{
            			    GPIO_PinWrite(GPIO_LEDred, Pin_LEDred, 0);
            			}
        				UC1701_printCentered(4, "                    ");
            			if ((keys & KEY_UP)!=0)
            			{
            				UC1701_printCentered(4, "Key: up");
            			}
            			else if ((keys & KEY_DOWN)!=0)
            			{
            				UC1701_printCentered(4, "Key: down");
            			}
            			else if ((keys & KEY_RIGHT)!=0)
            			{
            				UC1701_printCentered(4, "Key: right");
            			}
            			else if ((keys & KEY_LEFT)!=0)
            			{
            				UC1701_printCentered(4, "Key: left");
            			}
            			else if ((keys & KEY_GREEN)!=0)
            			{
            				UC1701_printCentered(4, "Key: green");
            			}
            			else if ((keys & KEY_RED)!=0)
            			{
            				UC1701_printCentered(4, "Key: red");
            			}
            			else if ((keys & KEY_STAR)!=0)
            			{
            				UC1701_printCentered(4, "Key: star");
            			}
            			else if ((keys & KEY_HASH)!=0)
            			{
            				UC1701_printCentered(4, "Key: hash");
            			}
            			else if ((keys & KEY_0)!=0)
            			{
            				UC1701_printCentered(4, "Key: 0");
            			}
            			else if ((keys & KEY_1)!=0)
            			{
            				UC1701_printCentered(4, "Key: 1");
            			}
            			else if ((keys & KEY_2)!=0)
            			{
            				UC1701_printCentered(4, "Key: 2");
            			}
            			else if ((keys & KEY_3)!=0)
            			{
            				UC1701_printCentered(4, "Key: 3");
            			}
            			else if ((keys & KEY_4)!=0)
            			{
            				UC1701_printCentered(4, "Key: 4");
            			}
            			else if ((keys & KEY_5)!=0)
            			{
            				UC1701_printCentered(4, "Key: 5");
            			}
            			else if ((keys & KEY_6)!=0)
            			{
            				UC1701_printCentered(4, "Key: 6");
            			}
            			else if ((keys & KEY_7)!=0)
            			{
            				UC1701_printCentered(4, "Key: 7");
            			}
            			else if ((keys & KEY_8)!=0)
            			{
            				UC1701_printCentered(4, "Key: 8");
            			}
            			else if ((keys & KEY_9)!=0)
            			{
            				UC1701_printCentered(4, "Key: 9");
            			}
            			Display_light_Touched = true;
            		}
        		}

            	if ((buttons & BUTTON_SK1)!=0) // Exit Testmode after 2 seconds
            	{
            		testmode--;
            		if (testmode==0)
            		{
            			reset_testmode();
                		show_running();
            		}
            	}
            	else
            	{
            		testmode = 2000;
            	}
        	}

        	if (testmode == 0)
        	{
            	if ((current_menu_level==-1) && ((buttons & BUTTON_SK1)!=0) && (button_event==EVENT_BUTTON_CHANGE)) // Enter Testmode
            	{
            		reset_splashscreen();
            		testmode = 2000;
            		show_testmode();
            	}
            	else if ((current_menu_level==-1) && ((buttons & BUTTON_SK2)!=0) && (button_event==EVENT_BUTTON_CHANGE)) // Enter Menu
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
            		show_running();
            	}
            	else if (current_menu_level>=0)
            	{
                	if (((keys & KEY_RIGHT)!=0) && (key_event==EVENT_KEY_CHANGE)) // Menu item right
                	{
                		menu_levels[current_menu_level].current_menu_item++;
                		if ( menu_levels[current_menu_level].current_menu[menu_levels[current_menu_level].current_menu_item].menu_item_text == NULL)
                		{
                			menu_levels[current_menu_level].current_menu_item = 0;
                		}
                		update_menu();
                	}
                	else if (((keys & KEY_LEFT)!=0) && (key_event==EVENT_KEY_CHANGE)) // Menu item left
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
                		update_menu();
                	}
                	else if (((keys & KEY_DOWN)!=0) && (key_event==EVENT_KEY_CHANGE)) // Menu level down
                	{
                		if (current_menu_level<MAX_MENU_LEVELS-1)
    					{
                    		menu_item_t* tmp_sub_menu = menu_levels[current_menu_level].current_menu[menu_levels[current_menu_level].current_menu_item].sub_menu;
                    		if (tmp_sub_menu!=NULL)
                    		{
                    			current_menu_level++;
                        		menu_levels[current_menu_level].current_menu = tmp_sub_menu;
                        		menu_levels[current_menu_level].current_menu_item = 0;
                        		update_menu();
                    		}
    					}
                	}
                	else if (((keys & KEY_UP)!=0) && (key_event==EVENT_KEY_CHANGE)) // Menu level up
                	{
                		if (current_menu_level>0)
                		{
                    		current_menu_level--;
                    		update_menu();
                		}
                	}
            	}
        	}
    	}

    	if ((GPIO_PinRead(GPIO_Power_Switch, Pin_Power_Switch)!=0) && (!Shutdown))
    	{
    		reset_splashscreen();
    		reset_menu();
    		reset_testmode();
    		show_poweroff();
    		Shutdown=true;
			Shutdown_Timer = 2000;
    	}
    	else if ((GPIO_PinRead(GPIO_Power_Switch, Pin_Power_Switch)==0) && (Shutdown))
    	{
			show_running();
			Shutdown=false;
			Shutdown_Timer = 0;
    	}

    	if (Shutdown)
    	{
    		if (Shutdown_Timer>0)
    		{
    			Shutdown_Timer--;
    			if (Shutdown_Timer==0)
    			{
    				GPIO_PinWrite(GPIO_Keep_Power_On, Pin_Keep_Power_On, 0);
    			}
    		}
    	}

		if (Show_SplashScreen)
		{
			show_splashscreen();
			SplashScreen_Timer = 4000;
			Show_SplashScreen = false;
		}

		if (SplashScreen_Timer>0)
		{
			SplashScreen_Timer--;
			if (SplashScreen_Timer==0)
			{
				show_running();
			}
		}

		if (Display_light_Touched)
		{
			if (Display_light_Timer==0)
			{
				GPIO_PinWrite(GPIO_Display_Light, Pin_Display_Light, 1);
			}
			Display_light_Timer = 4000;
			Display_light_Touched = false;
		}

		if (Display_light_Timer>0)
		{
			Display_light_Timer--;
			if (Display_light_Timer==0)
			{
				GPIO_PinWrite(GPIO_Display_Light, Pin_Display_Light, 0);
			}
		}

		vTaskDelay(portTICK_PERIOD_MS);
    }
}
