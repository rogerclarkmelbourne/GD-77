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
int current_frequency[FREQ_COUNT];
int current_frequency_idx;

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

bool check_frequency(int tmp_frequency)
{
	return ((tmp_frequency>=VHF_MIN) && (tmp_frequency<=VHF_MAX)) || ((tmp_frequency>=UHF_MIN) && (tmp_frequency<=UHF_MAX));
}

bool check_frequency_is_VHF()
{
	return ((current_frequency[current_frequency_idx]>=VHF_MIN) && (current_frequency[current_frequency_idx]<=VHF_MAX));
}

bool check_frequency_is_UHF()
{
	return ((current_frequency[current_frequency_idx]>=UHF_MIN) && (current_frequency[current_frequency_idx]<=UHF_MAX));
}

void update_frequency(int tmp_frequency)
{
	current_frequency[current_frequency_idx]=tmp_frequency;
	trx_set_mode_band_freq_and_others();
}

void update_flags()
{
	if (open_squelch)
	{
		UC1701_printAt(1*6, 0, "SQ:OFF");
	}
	else
	{
		UC1701_printAt(1*6, 0, "SQ:ON ");
	}
	if (HR_C6000_datalogging)
	{
		UC1701_printAt(14*6, 0, "LOG:ON ");
	}
	else
	{
		UC1701_printAt(14*6, 0, "LOG:OFF");
	}
}

typedef struct dmrIdData
{
	int id;
	char text[16];
} dmrIdRecord;

int int2bcd(int i)
{
    int result = 0;
    int shift = 0;

    while (i)
    {
        result +=  (i % 10) << shift;
        i = i / 10;
        shift += 4;
    }
    return result;
}

bool dmrIDLookup( int targetId,dmrIdRecord *foundRecord)
{
	uint32_t l = 0;
	uint32_t numRecords;
	uint32_t r;
	uint32_t m;

	uint32_t recordLenth;//15+4;
	uint8_t headerBuf[32];

	targetId=int2bcd(targetId);

	SPI_Flash_read(0x30000,headerBuf,0x0c);

	if (headerBuf[0] != 'I' || headerBuf[1] != 'D' || headerBuf[2] != '-')
	{
		return false;
	}

	numRecords = (uint32_t) headerBuf[8] | (uint32_t) headerBuf[9] << 8 | (uint32_t)headerBuf[10] <<16 | (uint32_t)headerBuf[11] << 24 ;

	recordLenth = (uint32_t) headerBuf[3] - 0x4a;

	r = numRecords - 1;

	while (l <= r)
	{
		m = (l + r) >> 1;

		SPI_Flash_read(0x3000c + recordLenth*m,(uint8_t *)foundRecord,recordLenth);

		if (foundRecord->id < targetId)
		{
			l = m + 1;
		}
		else
		{
			if (foundRecord->id >targetId)
			{
				r = m - 1;
			}
			else
			{
				return true;
			}
		}
	}

	return false;
}

void update_qsodata()
{
	if (qsodata_timer>0)
	{
		char buffer[32];
		dmrIdRecord currentRec;

		sprintf(buffer,"TG %d", last_TG);
		UC1701_printCentered(5, buffer);
		if (!dmrIDLookup(last_DMRID,&currentRec))
		{
			sprintf(buffer,"DMRID %d", last_DMRID);
		}
		else
		{
			sprintf(buffer,"%s", currentRec.text);
		}
		UC1701_printCentered(6, buffer);
	}
	else
	{
		UC1701_printCentered(5, "                    ");
		UC1701_printCentered(6, "                    ");
	}
}

void update_screen()
{
	UC1701_clear();
	update_flags();
	update_qsodata();
	if (current_mode==MODE_ANALOG)
	{
		UC1701_printCentered(2, "analog");
	}
	else if (current_mode==MODE_DIGITAL)
	{
		UC1701_printCentered(2, "DMR");
	}
	char buffer[32];
	if (freq_enter_idx==0)
	{
		int val_before_dp = current_frequency[current_frequency_idx]/10000;
		int val_after_dp = current_frequency[current_frequency_idx] - val_before_dp*10000;
		sprintf(buffer,"#%d %d.%04d MHz", current_frequency_idx+1, val_before_dp, val_after_dp);
	}
	else
	{
		sprintf(buffer,"#%d %c%c%c.%c%c%c%c MHz", current_frequency_idx+1, freq_enter_digits[0], freq_enter_digits[1], freq_enter_digits[2], freq_enter_digits[3], freq_enter_digits[4], freq_enter_digits[5], freq_enter_digits[6] );
	}
	UC1701_printCentered(3, buffer);
	Display_light_Touched = true;
}

void init_edit()
{
	current_mode = MODE_DIGITAL;
	for (int i=0;i<FREQ_COUNT;i++)
	{
		current_frequency[i] = VHF_MIN;
	}
	current_frequency_idx=0;

	trx_set_mode_band_freq_and_others();

	reset_freq_enter_digits();
}

void tick_edit()
{
	if (freq_enter_idx==0)
	{
		if ((keys & KEY_RIGHT)!=0)
		{
			current_frequency_idx++;
			if (current_frequency_idx>FREQ_COUNT-1)
			{
				current_frequency_idx=0;
			}
			trx_set_mode_band_freq_and_others();
		}
		else if ((keys & KEY_LEFT)!=0)
		{
			current_frequency_idx--;
			if (current_frequency_idx<0)
			{
				current_frequency_idx=FREQ_COUNT-1;
			}
			trx_set_mode_band_freq_and_others();
		}
		else if ((keys & KEY_STAR)!=0)
		{
			if (current_mode == MODE_ANALOG)
			{
				current_mode = MODE_DIGITAL;
			}
			else if (current_mode == MODE_DIGITAL)
			{
				current_mode = MODE_ANALOG;
			}
			trx_set_mode_band_freq_and_others();
		}
		else if ((keys & KEY_DOWN)!=0)
		{
			int tmp_frequency=current_frequency[current_frequency_idx]-FREQ_STEP;
			if (check_frequency(tmp_frequency))
			{
				update_frequency(tmp_frequency);
			}
			else
			{
        	    set_melody(melody_ERROR_beep);
			}
		}
		else if ((keys & KEY_UP)!=0)
		{
			int tmp_frequency=current_frequency[current_frequency_idx]+FREQ_STEP;
			if (check_frequency(tmp_frequency))
			{
				update_frequency(tmp_frequency);
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
			if (check_frequency(tmp_frequency))
			{
				update_frequency(tmp_frequency);
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
