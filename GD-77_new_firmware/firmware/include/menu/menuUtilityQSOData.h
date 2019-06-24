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
#ifndef _MENU_UTILITY_QSO_DATA_H_
#define _MENU_UTILITY_QSO_DATA_H_                    /**< Symbol preventing repeated inclusion */
#include "fw_common.h"

#define NUM_LASTHEARD_STORED 8

typedef struct dmrIdDataStruct
{
	int id;
	char text[20];
} dmrIdDataStruct_t;


typedef struct LinkItem
{
    struct LinkItem *prev;
    int id;
    int talkGroup;
    char talkerAlias[32];// 4 blocks of data. 6 bytes + 6 bytes + 7 bytes + 7 bytes . plus 1 for termination some more for safety
    struct LinkItem *next;
} LinkItem_t;

enum QSO_DISPLAY_STATE
{
	QSO_DISPLAY_IDLE,
	QSO_DISPLAY_DEFAULT_SCREEN,
	QSO_DISPLAY_CALLER_DATA
};

extern LinkItem_t *LinkHead;
extern int menuDisplayQSODataState;

bool dmrIDLookup( int targetId,dmrIdDataStruct_t *foundRecord);
void menuUtilityRenderQSOData();
void menuUtilityRenderHeader();
void lastheardInitList();
void lastHeardListUpdate(uint8_t *dmrDataBuffer);
#endif
