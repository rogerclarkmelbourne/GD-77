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

#include "fw_settings.h"

int current_mode = 0;
int current_frequency[FREQ_COUNT];
int current_frequency_idx;

void save_value(int idx, int value)
{
	int tmp_value=load_value(idx);
	if (tmp_value!=value)
	{
		EEPROM_Write(STORAGE_BASE_ADDRESS+idx*sizeof(int), (uint8_t*)&value, sizeof(int));
	    vTaskDelay(portTICK_PERIOD_MS * 5);
	}
}

int load_value(int idx)
{
	int tmp_value=0;
	EEPROM_Read(STORAGE_BASE_ADDRESS+idx*sizeof(int), (uint8_t*)&tmp_value, sizeof(int));
	return tmp_value;
}

void save_settings()
{
	save_value(0, STORAGE_MAGIC_NUMBER);
	save_value(1, current_mode);
	save_value(2, current_frequency_idx);
	for (int i=0;i<FREQ_COUNT;i++)
	{
		save_value(3+i, current_frequency[i]);
	}
}

void load_settings()
{
	if (load_value(0)==STORAGE_MAGIC_NUMBER)
	{
		current_mode = load_value(1);
		current_frequency_idx = load_value(2);
		for (int i=0;i<FREQ_COUNT;i++)
		{
			current_frequency[i] = load_value(3+i);
		}
	}
	else
	{
		current_mode = MODE_DIGITAL;
		for (int i=0;i<FREQ_COUNT;i++)
		{
			current_frequency[i] = VHF_MIN;
		}
		current_frequency_idx=0;
	}
}
