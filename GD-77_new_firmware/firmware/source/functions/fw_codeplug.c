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
#include <fw_codeplug.h>
#include "fw_EEPROM.h"
#include "fw_SPI_Flash.h"

const int CODEPLUG_ADDR_EX_ZONE_BASIC = 0x8000;
const int CODEPLUG_ADDR_EX_ZONE_LIST =  0x8030;
const int CODEPLUG_ZONE_DATA_SIZE = 48;
const int CODEPLUG_ZONE_MAX_COUNT = 250;
const int CODEPLUG_CHANNEL_DATA_SIZE = 56;
const int CODEPLUG_ADDR_CHANNEL_EEPROM = 0x3790;
const int CODEPLUG_ADDR_CHANNEL_FLASH = 0x7B1C0;

const int CODEPLUG_ADDR_RX_GROUP = 0x8D6A0;//
const int CODEPLUG_RX_GROUP_LEN = 0x50;

const int CODEPLUG_ADDR_CONTACTS = 0x87620;
const int CODEPLUG_CONTACTS_LEN = 0x18;

const int CODEPLUG_ADDR_USER_DMRID = 0x00E8;
const int CODEPLUG_ADDR_USER_CALLSIGN = 0x00E0;

const int CODEPLUG_ADDR_BOOT_LINE1 = 0x7540;
const int CODEPLUG_ADDR_BOOT_LINE2 = 0x7550;

uint32_t byteSwap32(uint32_t n)
{
    return ((((n)&0x000000FFU) << 24U) | (((n)&0x0000FF00U) << 8U) | (((n)&0x00FF0000U) >> 8U) | (((n)&0xFF000000U) >> 24U));// from usb_misc.h
}

// BCD encoding to integer conversion
uint32_t bcd2int(uint32_t i)
{
    int result = 0;
    int multiplier = 1;
    while (i)
    {
        result +=  (i &0x0f)* multiplier;
        multiplier *=10;
        i = i >> 4;
    }
    return result;
}

void codeplugUtilConvertBufToString(char *inBuf,char *outBuf,int len)
{
	for(int i=0;i<len;i++)
	{
		if (inBuf[i]==0xff)
		{
			inBuf[i]=0;
		}
		outBuf[i]=inBuf[i];
	}
	outBuf[len]=0;
	return;
}
/*
 * deprecated. Use our own non volatile storage instead
 *
void codeplugZoneGetSelected(int *selectedZone,int *selectedChannel)
{
	uint8_t buf[4];
	EEPROM_Read(CODEPLUG_ADDR_EX_ZONE_BASIC + 4, (uint8_t*)&buf, sizeof(buf));
	*selectedZone = buf[2];
	*selectedChannel = buf[0];
}

void codeplugZoneSetSelected(int selectedZone,int selectedChannel)
{
	uint8_t buf[4]={selectedChannel,0,selectedZone,0};
	EEPROM_Write(CODEPLUG_ADDR_EX_ZONE_BASIC + 4, (uint8_t*)&buf, sizeof(buf));
    vTaskDelay(portTICK_PERIOD_MS * 5);// Not essential since the user will not be able to retrigger this in less than 5mS
}
*/

int codeplugZonesGetCount()
{
	struct_codeplugZone_t buf;
	int z;

	for(z=0;z<CODEPLUG_ZONE_MAX_COUNT;z++)
	{
		// Note. The struct contains an additional field not stored in the memory, hence why the use of CODEPLUG_ZONE_DATA_SIZE instead of
		// sizeof(struct_codeplugZone_t)
		EEPROM_Read(CODEPLUG_ADDR_EX_ZONE_LIST + (z * CODEPLUG_ZONE_DATA_SIZE), (uint8_t*)&buf, CODEPLUG_ZONE_DATA_SIZE);
		if (buf.name[0] == 0xff || buf.channels[0]==0 )
		{
			return (z-1);
		}
	}
	return z;
}

void codeplugZoneGetDataForIndex(int index,struct_codeplugZone_t *returnBuf)
{
	// IMPORTANT. read size is different from the size of the data, because I added a extra property to the struct to hold the number of channels in the zone.
	EEPROM_Read(CODEPLUG_ADDR_EX_ZONE_LIST + (index * CODEPLUG_ZONE_DATA_SIZE), (uint8_t*)returnBuf, sizeof(struct_codeplugZone_t));
	for(int i=0;i<32;i++)
	{
		// Empty channels seem to be filled with zeros
		if (returnBuf->channels[i] == 0)
		{
			returnBuf->NOT_IN_MEMORY_numChannelsInZone = i;
			return;
		}
	}
	returnBuf->NOT_IN_MEMORY_numChannelsInZone=32;
}

void codeplugChannelGetDataForIndex(int index, struct_codeplugChannel_t *channelBuf)
{
	// lower 128 channels are in EEPROM. Remaining channels are in Flash ! (What a mess...)
	index--; // I think the channel index numbers start from 1 not zero.
	if (index<128)
	{
		EEPROM_Read(CODEPLUG_ADDR_CHANNEL_EEPROM + index*sizeof(struct_codeplugChannel_t),(uint8_t *)channelBuf,sizeof(struct_codeplugChannel_t));
	}
	else
	{
		int flashReadPos = CODEPLUG_ADDR_CHANNEL_FLASH;

		if (index>255)
		{
			flashReadPos+=16;// some sort of break in the blocks after the first 255. Not sure if this happens every 256 or just in the first (arrgghhh)
		}
		index -= 128;
		SPI_Flash_read(flashReadPos + index*sizeof(struct_codeplugChannel_t),(uint8_t *)channelBuf,sizeof(struct_codeplugChannel_t));
	}

	// Convert the the legacy codeplug tx and rx freq values into normal integers
	channelBuf->txFreq = bcd2int(channelBuf->txFreq)/10;
	channelBuf->rxFreq = bcd2int(channelBuf->rxFreq)/10;
}


void codeplugRxGroupGetDataForIndex(int index, struct_codeplugRxGroup_t *rxGroupBuf)
{
	int i=0;
	index--; //Index numbers start from 1 not zero
// Not our struct contains an extra property to hold the number of TGs in the group
	SPI_Flash_read(CODEPLUG_ADDR_RX_GROUP + index*(sizeof(struct_codeplugRxGroup_t) - sizeof(int)),(uint8_t *)rxGroupBuf,sizeof(struct_codeplugRxGroup_t) - sizeof(int));
	for(i=0;i<32;i++)
	{
		// Empty groups seem to be filled with zeros
		if (rxGroupBuf->contacts[i] == 0)
		{
			break;
		}
	}
	rxGroupBuf->NOT_IN_MEMORY_numTGsInGroup = i;
}

void codeplugContactGetDataForIndex(int index, struct_codeplugContact_t *contact)
{
	index--;
	SPI_Flash_read(CODEPLUG_ADDR_CONTACTS + index*sizeof(struct_codeplugContact_t),(uint8_t *)contact,sizeof(struct_codeplugContact_t));
	contact->tgNumber = bcd2int(byteSwap32(contact->tgNumber));
}

int codeplugGetUserDMRID()
{
	int dmrId;
	EEPROM_Read(CODEPLUG_ADDR_USER_DMRID,(uint8_t *)&dmrId,4);
	return bcd2int(byteSwap32(dmrId));
}

void codeplugGetRadioName(char *buf)
{
	EEPROM_Read(CODEPLUG_ADDR_USER_CALLSIGN,(uint8_t *)buf,6);
	codeplugUtilConvertBufToString(buf,buf,6);
}

void codeplugGetBootItemTexts(char *line1, char *line2)
{
	memset(line1,0,16);
	memset(line2,0,16);

	EEPROM_Read(CODEPLUG_ADDR_BOOT_LINE1,(uint8_t *)line1,15);
	codeplugUtilConvertBufToString(line1,line1,15);
	EEPROM_Read(CODEPLUG_ADDR_BOOT_LINE2,(uint8_t *)line2,15);
	codeplugUtilConvertBufToString(line2,line2,15);
}
