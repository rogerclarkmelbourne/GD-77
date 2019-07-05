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
#ifndef _FW_MENU_LEGACY_CODEPLUG_UTILS_H_
#define _FW_MENU_LEGACY_CODEPLUG_UTILS_H_
#include "fw_main.h"

extern const int CODEPLUG_ZONE_DATA_SIZE;

typedef struct struct_codeplugZone
{
	char name[16];
	uint16_t channels[16];
	int	NOT_IN_MEMORY_numChannelsInZone;// NOT IN THE
}
struct_codeplugZone_t;

typedef struct struct_codeplugChannel
{
	char name[16];
	uint32_t rxFreq;
	uint32_t txFreq;
	uint8_t chMode;
	uint8_t rxRefFreq;
	uint8_t txRefFreq;
	uint8_t tot;
	uint8_t totRekey;
	uint8_t admitCriteria;
	uint8_t rssiThreshold;
	uint8_t scanList;
	uint16_t rxTone;
	uint16_t txTone;
	uint8_t voiceEmphasis;
	uint8_t txSignaling;
	uint8_t unmuteRule;
	uint8_t rxSignaling;
	uint8_t artsInterval;
	uint8_t encrypt;
	uint8_t rxColor;
	uint8_t rxGroupList;
	uint8_t txColor;
	uint8_t emgSystem;
	uint16_t contact;
	uint8_t flag1;
	uint8_t flag2;
	uint8_t flag3;
	uint8_t flag4;// bits... 0x80 = Power, 0x40 = Vox, 0x20 = AutoScan, 0x10 = LoneWoker, 0x08 = AllowTalkaround, 0x04 = OnlyRx, 0x02 = Channel width, 0x01 = Squelch
	uint16_t reserve2;
	uint8_t reserve;
	uint8_t sql;
} struct_codeplugChannel_t;

typedef struct struct_codeplugRxGroup
{
	char name[16];
	uint16_t contacts[32];
	int	NOT_IN_MEMORY_numTGsInGroup;// NOT IN THE
} struct_codeplugRxGroup_t;

typedef struct struct_codeplugContact
{
	char name[16];
	uint32_t tgNumber;
	uint16_t flags; // probably used for call type, but possibly can be re-purposed
} struct_codeplugContact_t;

/*
 * deprecated. Use our own non volatile storage instead
 *
void codeplugZoneGetSelected(int *selectedZone,int *selectedChannel);
void codeplugZoneSetSelected(int selectedZone,int selectedChannel);
 */
int codeplugZonesGetCount();
void codeplugZoneGetDataForIndex(int indexNum,struct_codeplugZone_t *returnBuf);

void codeplugChannelGetDataForIndex(int index, struct_codeplugChannel_t *channelBuf);
void codeplugUtilConvertBufToString(char *inBuf,char *outBuf,int len);
uint32_t byteSwap32(uint32_t n);
uint32_t bcd2int(uint32_t in);

void codeplugRxGroupGetDataForIndex(int index, struct_codeplugRxGroup_t *rxGroupBuf);
void codeplugContactGetDataForIndex(int index, struct_codeplugContact_t *contact);
int codeplugGetUserDMRID();
void codeplugGetRadioName(char *buf);
void codeplugGetBootItemTexts(char *line1, char *line2);
void codeplugVFO_A_ChannelData(struct_codeplugChannel_t *vfoBuf);

#endif
