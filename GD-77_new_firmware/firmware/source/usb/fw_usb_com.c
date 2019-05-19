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

#include "fw_usb_com.h"

uint8_t tmp_val_0x82;
uint8_t tmp_val_0x86;
uint8_t tmp_val_0x51;
uint8_t tmp_val_0x52;
uint8_t tmp_val_0x57;
uint8_t tmp_val_0x5f;
uint8_t tmp_ram[256];
uint8_t tmp_ram1[256];
uint8_t tmp_ram2[256];

uint8_t com_buffer[COM_BUFFER_SIZE];
int com_buffer_write_idx = 0;
int com_buffer_read_idx = 0;
int com_buffer_cnt = 0;

int com_request = 0;
uint8_t com_requestbuffer[COM_REQUESTBUFFER_SIZE];
USB_DMA_NONINIT_DATA_ALIGN(USB_DATA_ALIGN_SIZE) static uint8_t s_ComBuf[DATA_BUFF_SIZE];

static uint8_t sectorbuffer[4096];
int sector = -1;

void tick_com_request()
{
	if (com_request==1)
	{
		if (com_requestbuffer[0]=='R') // 'R' read data (com_requestbuffer[1]: 1 => external flash, 2 => EEPROM)
		{
			uint32_t address=(com_requestbuffer[2]<<24)+(com_requestbuffer[3]<<16)+(com_requestbuffer[4]<<8)+(com_requestbuffer[5]<<0);
			uint32_t length=(com_requestbuffer[6]<<8)+(com_requestbuffer[7]<<0);
			if (length>32)
			{
				length=32;
			}

			bool result;
			if (com_requestbuffer[1]==1)
			{
				result = SPI_Flash_read(address, &s_ComBuf[3], length);
			}
			else if (com_requestbuffer[1]==2)
			{
				result = EEPROM_Read(address, &s_ComBuf[3], length);
			}

			if (result)
			{
				s_ComBuf[0] = com_requestbuffer[0];
				s_ComBuf[1]=(length>>8)&0xFF;
				s_ComBuf[2]=(length>>0)&0xFF;
				USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle, USB_CDC_VCOM_BULK_IN_ENDPOINT, s_ComBuf, length+3);
			}
			else
			{
				s_ComBuf[0] = '-';
				USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle, USB_CDC_VCOM_BULK_IN_ENDPOINT, s_ComBuf, 1);
			}
		}
		else if (com_requestbuffer[0]=='W')
		{
			bool ok=false;
			if (com_requestbuffer[1]==1)
			{
				if (sector==-1)
				{
					sector=(com_requestbuffer[2]<<16)+(com_requestbuffer[3]<<8)+(com_requestbuffer[4]<<0);
					ok = SPI_Flash_read(sector*4096,sectorbuffer,4096);
				}
			}
			else if (com_requestbuffer[1]==2)
			{
				if (sector>=0)
				{
					uint32_t address=(com_requestbuffer[2]<<24)+(com_requestbuffer[3]<<16)+(com_requestbuffer[4]<<8)+(com_requestbuffer[5]<<0);
					uint32_t length=(com_requestbuffer[6]<<8)+(com_requestbuffer[7]<<0);
					if (length>32)
					{
						length=32;
					}

					for (int i=0;i<length;i++)
					{
						if (sector==(address+i)/4096)
						{
							sectorbuffer[(address+i) % 4096]=com_requestbuffer[i+8];
						}
					}

					ok=true;
				}
			}
			else if (com_requestbuffer[1]==3)
			{
				if (sector>=0)
				{
					ok = SPI_Flash_eraseSector(sector*4096);
					if (ok)
					{
						for (int i=0;i<16;i++)
						{
							ok = SPI_Flash_writePage(sector*4096+i*256,sectorbuffer+i*256);
							if (!ok)
							{
								break;
							}
						}
					}
					sector=-1;
				}
			}

			if (ok)
			{
				s_ComBuf[0] = com_requestbuffer[0];
				s_ComBuf[1] = com_requestbuffer[1];
				USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle, USB_CDC_VCOM_BULK_IN_ENDPOINT, s_ComBuf, 2);
			}
			else
			{
				sector=-1;
				s_ComBuf[0] = '-';
				USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle, USB_CDC_VCOM_BULK_IN_ENDPOINT, s_ComBuf, 1);
			}
		}
		else
		{
			s_ComBuf[0] = '-';
			USB_DeviceCdcAcmSend(s_cdcVcom.cdcAcmHandle, USB_CDC_VCOM_BULK_IN_ENDPOINT, s_ComBuf, 1);
		}
		com_request=0;
	}
}

void send_packet(uint8_t val_0x82, uint8_t val_0x86, int ram)
{
	if ((HR_C6000_datalogging) && ((com_buffer_cnt+8+(ram+1))<=COM_BUFFER_SIZE))
	{
		taskENTER_CRITICAL();
		add_to_commbuffer((com_buffer_cnt >> 8) & 0xff);
		add_to_commbuffer((com_buffer_cnt >> 0) & 0xff);
		add_to_commbuffer(val_0x82);
		add_to_commbuffer(val_0x86);
		add_to_commbuffer(tmp_val_0x51);
		add_to_commbuffer(tmp_val_0x52);
		add_to_commbuffer(tmp_val_0x57);
		add_to_commbuffer(tmp_val_0x5f);
		for (int i=0;i<=ram;i++)
		{
			add_to_commbuffer(tmp_ram[i]);
		}
		taskEXIT_CRITICAL();
	}
}

void send_packet_big(uint8_t val_0x82, uint8_t val_0x86, int ram1, int ram2)
{
	if ((HR_C6000_datalogging) && ((com_buffer_cnt+8+(ram1+1)+(ram2+1))<=COM_BUFFER_SIZE))
	{
		taskENTER_CRITICAL();
		add_to_commbuffer((com_buffer_cnt >> 8) & 0xff);
		add_to_commbuffer((com_buffer_cnt >> 0) & 0xff);
		add_to_commbuffer(val_0x82);
		add_to_commbuffer(val_0x86);
		add_to_commbuffer(tmp_val_0x51);
		add_to_commbuffer(tmp_val_0x52);
		add_to_commbuffer(tmp_val_0x57);
		add_to_commbuffer(tmp_val_0x5f);
		for (int i=0;i<=ram1;i++)
		{
			add_to_commbuffer(tmp_ram1[i]);
		}
		for (int i=0;i<=ram2;i++)
		{
			add_to_commbuffer(tmp_ram2[i]);
		}
		taskEXIT_CRITICAL();
	}
}

void add_to_commbuffer(uint8_t value)
{
	com_buffer[com_buffer_write_idx]=value;
	com_buffer_cnt++;
	com_buffer_write_idx++;
	if (com_buffer_write_idx==COM_BUFFER_SIZE)
	{
		com_buffer_write_idx=0;
	}
}
