/*
 * Initial work for port to MK22FN512xxx12 Copyright (C)2019 Kai Ludwig, DG4KLU
 *
 * Code mainly re-written by Roger Clark. VK3KYY / G4KYF
 * based on information and code references from various sources, including
 * https://github.com/bitbank2/uc1701 and
 * https://os.mbed.com/users/Anaesthetix/code/UC1701/file/7494bdca926b/
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

#ifndef __UC1701_H__
#define __UC1701_H__

#include "FreeRTOS.h"
#include "task.h"
#include "fw_common.h"

#define UC1701_FONT_6X8 				0
#define UC1701_FONT_6X8_bold			1
#define UC1701_FONT_8X8 				2
#define UC1701_FONT_GD77_8x16 			3
#define UC1701_FONT_16x32 				4

void UC1701_begin(bool isInverted);
void UC1701_clearBuf();
void UC1701_render();
void UC1701_printCentered(uint8_t y, char *text,int fontSize);
void UC1701_printAt(uint8_t x, uint8_t y, char *text,int fontSize);
int UC1701_printCore(int x, int y, char *szMsg, int iSize, int alignment, bool isInverted);
int UC1701_setPixel(int x, int y, bool color);
void UC1701_fillRect(int x,int y,int width,int height,bool isInverted);
void UC1701_setContrast(uint8_t contrast);

#endif /* __UC1701_H__ */
