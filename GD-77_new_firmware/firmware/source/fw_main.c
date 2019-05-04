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
#include "fw_i2c.h"
#include "fw_spi.h"
#include "fw_i2s.h"
#include "fw_AT1846S.h"
#include "fw_HR-C6000.h"

#include "UC1701.h"

#include "fw_sound.h"

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
	UC1701_printCentered(2, "firmware V0.0.1");
	UC1701_printCentered(4, "by");
	UC1701_printCentered(6, "DG4KLU");
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

#define MODE_SILENT 0
#define MODE_ANALOG 1
#define MODE_DIGITAL 2

#define BAND_VHF 1
#define BAND_UHF 2

#define VHF_MIN 1440000
#define VHF_MAX 1459999
#define UHF_MIN 4300000
#define UHF_MAX 4399999

#define FREQ_STEP 125

int current_mode = 0;
int current_band = 0;
int current_VHF_frequency = 0;
int current_UHF_frequency = 0;
int current_frequency = 0;

char freq_enter_digits[7] = { '-', '-', '-', '-', '-', '-', '-' };
int freq_enter_idx = 0;

void reset_freq_enter_digits()
{
	for (int i=0;i<7;i++)
	{
		freq_enter_digits[i]='-';
	}
	freq_enter_idx = 0;
}

int read_freq_enter_digits()
{
	int result=0;
	for (int i=0;i<7;i++)
	{
		result=result*10;
		if ((freq_enter_digits[i]>='0') && (freq_enter_digits[i]<='9'))
		{
			result=result+freq_enter_digits[i]-'0';
		}
	}
	return result;
}

bool check_frequency(int tmp_band, int tmp_frequency)
{
	return ((tmp_band == BAND_VHF) && (tmp_frequency>=VHF_MIN) && (tmp_frequency<=VHF_MAX)) || ((tmp_band == BAND_UHF) && (tmp_frequency>=UHF_MIN) && (tmp_frequency<=UHF_MAX));
}

void update_band_and_frequency(int tmp_band, int tmp_frequency)
{
	current_band=tmp_band;
	current_frequency=tmp_frequency;
	if (current_band == BAND_VHF)
	{
		current_VHF_frequency=current_frequency;
	}
	else if (current_band == BAND_UHF)
	{
		current_UHF_frequency=current_frequency;
	}
}

void update_screen()
{
	UC1701_clear();
	if (current_mode==MODE_SILENT)
	{
		UC1701_printCentered(2, "Silent");
	}
	else if (current_mode==MODE_ANALOG)
	{
		UC1701_printCentered(2, "FM analog");
	}
	else if (current_mode==MODE_DIGITAL)
	{
		UC1701_printCentered(2, "DMR");
	}
	char buffer[32];
	if (freq_enter_idx==0)
	{
		int val_before_dp = current_frequency/10000;
		int val_after_dp = current_frequency - val_before_dp*10000;
		sprintf(buffer,"%d.%04d MHz", val_before_dp, val_after_dp);
	}
	else
	{
		sprintf(buffer,"%c%c%c.%c%c%c%c MHz", freq_enter_digits[0], freq_enter_digits[1], freq_enter_digits[2], freq_enter_digits[3], freq_enter_digits[4], freq_enter_digits[5], freq_enter_digits[6] );
	}
	UC1701_printCentered(5, buffer);
	Display_light_Touched = true;
}

void fw_main_task()
{
    USB_DeviceApplicationInit();

	fw_init_common();
	fw_init_buttons();
	fw_init_LEDs();
	fw_init_keyboard();
	fw_init_display();

    // Init I2C
    init_I2C0a();
    setup_I2C0();

    // Init SPI
    init_SPI();
    setup_SPI0();
    setup_SPI1();

    // Init I2S
    init_I2S();
    setup_I2S();

    // Init AT1846S
    I2C_AT1846S_init();

    // Init HR-C6000
    SPI_HR_C6000_init();

    // Small startup delay after initialization to stabilize system
    vTaskDelay(portTICK_PERIOD_MS * 500);

    fw_init_beep_task();

    set_melody(melody_poweron);

	Show_SplashScreen = true;

	reset_menu();

	current_mode = MODE_SILENT;
	current_band = BAND_VHF;
	current_VHF_frequency = VHF_MIN;
	current_UHF_frequency = UHF_MIN;
	current_frequency = VHF_MIN;

	reset_freq_enter_digits();

    while (1U)
    {
    	// Read button state and event
    	fw_check_button_event(&buttons, &button_event);

    	// Read keyboard state and event
    	fw_check_key_event(&keys, &key_event);

    	if (key_event==EVENT_KEY_CHANGE)
    	{
    		if (keys!=0)
    		{
        	    set_melody(melody_key_beep);

        	    if (current_menu_level==-1)
        	    {
        	    	if (freq_enter_idx==0)
        	    	{
        				if ((keys & KEY_HASH)!=0)
        				{
        					if (current_band == BAND_VHF)
        					{
        						update_band_and_frequency(BAND_UHF, current_UHF_frequency);
        					}
        					else if (current_band == BAND_UHF)
        					{
        						update_band_and_frequency(BAND_VHF, current_VHF_frequency);
        					}
        				}
        				else if ((keys & KEY_STAR)!=0)
        				{
        					if (current_mode == MODE_SILENT)
        					{
        						current_mode = MODE_ANALOG;
        					}
        					else if (current_mode == MODE_ANALOG)
        					{
        						current_mode = MODE_DIGITAL;
        					}
        					else if (current_mode == MODE_DIGITAL)
        					{
        						current_mode = MODE_SILENT;
        					}
        				}
        				else if ((keys & KEY_DOWN)!=0)
        				{
        					int tmp_frequency=current_frequency-FREQ_STEP;
        					if (check_frequency(current_band, tmp_frequency))
        					{
        						update_band_and_frequency(current_band, tmp_frequency);
        					}
        					else
        					{
            	        	    set_melody(melody_ERROR_beep);
        					}
        				}
        				else if ((keys & KEY_UP)!=0)
        				{
        					int tmp_frequency=current_frequency+FREQ_STEP;
        					if (check_frequency(current_band, tmp_frequency))
        					{
        						update_band_and_frequency(current_band, tmp_frequency);
        					}
        					else
        					{
            	        	    set_melody(melody_ERROR_beep);
        					}
        				}
        	    	}
        	    	else
        	    	{
        				if ((keys & KEY_LEFT)!=0)
        				{
        	    			freq_enter_idx--;
        	    			freq_enter_digits[freq_enter_idx]='-';
        				}
        				else if ((keys & KEY_GREEN)!=0)
        				{
        					int tmp_frequency=read_freq_enter_digits();
        					int tmp_band=0;
        					if ((tmp_frequency>=VHF_MIN) && (tmp_frequency<=VHF_MAX))
        					{
        						tmp_band=BAND_VHF;
        					}
        					else if ((tmp_frequency>=UHF_MIN) && (tmp_frequency<=UHF_MAX))
        					{
        						tmp_band=BAND_UHF;
        					}
        					if (check_frequency(tmp_band, tmp_frequency))
        					{
        						update_band_and_frequency(tmp_band, tmp_frequency);
            					reset_freq_enter_digits();
            	        	    set_melody(melody_ACK_beep);
        					}
        					else
        					{
            	        	    set_melody(melody_ERROR_beep);
        					}
        				}
        				else if ((keys & KEY_RED)!=0)
        				{
        					reset_freq_enter_digits();
        	        	    set_melody(melody_NACK_beep);
        				}
        	    	}
        	    	if (freq_enter_idx<7)
        	    	{
        	    		char c='\0';
        				if ((keys & KEY_0)!=0)
        				{
        	    			c='0';
        				}
        				else if ((keys & KEY_1)!=0)
        				{
        	    			c='1';
        				}
        				else if ((keys & KEY_2)!=0)
        				{
        	    			c='2';
        				}
        				else if ((keys & KEY_3)!=0)
        				{
        	    			c='3';
        				}
        				else if ((keys & KEY_4)!=0)
        				{
        	    			c='4';
        				}
        				else if ((keys & KEY_5)!=0)
        				{
        	    			c='5';
        				}
        				else if ((keys & KEY_6)!=0)
        				{
        	    			c='6';
        				}
        				else if ((keys & KEY_7)!=0)
        				{
        	    			c='7';
        				}
        				else if ((keys & KEY_8)!=0)
        				{
        	    			c='8';
        				}
        				else if ((keys & KEY_9)!=0)
        				{
        	    			c='9';
        				}
        	    		if (c!='\0')
        	    		{
        	    			freq_enter_digits[freq_enter_idx]=c;
        	    			freq_enter_idx++;
        	    		}
        	    	}

            	    update_screen();
        	    }
    		}
    	}

    	if (button_event==EVENT_BUTTON_CHANGE)
    	{
    		if ((buttons & BUTTON_SK1)!=0)
    		{
        	    set_melody(melody_sk1_beep);
    		}
    		else if ((buttons & BUTTON_SK2)!=0)
    		{
        	    set_melody(melody_sk2_beep);
    		}
    		else if ((buttons & BUTTON_ORANGE)!=0)
    		{
        	    set_melody(melody_orange_beep);
    		}
    	}

    	if (!Shutdown)
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

    	if ((GPIO_PinRead(GPIO_Power_Switch, Pin_Power_Switch)!=0) && (!Shutdown))
    	{
    		reset_splashscreen();
    		reset_menu();
    		show_poweroff();
		    GPIO_PinWrite(GPIO_speaker_mute, Pin_speaker_mute, 0);
		    set_melody(NULL);
    		Shutdown=true;
			Shutdown_Timer = 2000;
    	}
    	else if ((GPIO_PinRead(GPIO_Power_Switch, Pin_Power_Switch)==0) && (Shutdown))
    	{
			update_screen();
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
				update_screen();
			}
			else if (key_event!=EVENT_KEY_CHANGE)
			{
				show_splashscreen();
				SplashScreen_Timer = 4000;
			}
			Show_SplashScreen = false;
		}

		if (SplashScreen_Timer>0)
		{
			SplashScreen_Timer--;
			if (SplashScreen_Timer==0)
			{
				update_screen();
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

		tick_melody();

		vTaskDelay(portTICK_PERIOD_MS);
    }
}
