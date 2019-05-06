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

#include "fw_edit.h"

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

void init_edit()
{
	current_mode = MODE_SILENT;
	current_band = BAND_VHF;
	current_VHF_frequency = VHF_MIN;
	current_UHF_frequency = UHF_MIN;
	current_frequency = VHF_MIN;

	reset_freq_enter_digits();
}

void tick_edit()
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
