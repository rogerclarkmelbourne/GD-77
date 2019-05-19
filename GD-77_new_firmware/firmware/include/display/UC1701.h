/*
 * UC7101 - Interface with UC7101 (or compatible) LCDs.
 *
 * Copyright (c) 2014 Rustem Iskuzhin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * Additional work for port to MK22FN512xxx12 Copyright (C)2019 Kai Ludwig, DG4KLU
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

#define UC1701_COMMAND 0
#define UC1701_DATA    1

// Send a command or data to the display...
void UC1701_transfer(uint8_t mode, int data1);

// Display initialization (dimensions in pixels)...
void UC1701_begin();

// Erase everything on the display...
void UC1701_clear();
void UC1701_clearLine();  // ...or just the current line

// Place the cursor at the start of the current line...
void UC1701_home();

// Place the cursor at position (column, line)...
void UC1701_setCursor(uint8_t column, uint8_t line);

// Assign a user-defined glyph (5x8) to an ASCII character (0-31)...
void UC1701_createChar(uint8_t chr, const uint8_t *glyph);

// Print an ASCII string centered at line y (7-bit)...
void UC1701_printCentered(uint8_t y, char *text);

// Print an ASCII string at cursor position x,y (7-bit)...
void UC1701_printAt(uint8_t x, uint8_t y, char *text);

// Print an ASCII string at the current cursor position (7-bit)...
void UC1701_print(char *text);

// Write an ASCII character at the current cursor position (7-bit)...
int UC1701_write(uint8_t chr);

// Draw a bitmap at the current cursor position...
void UC1701_drawBitmap(const uint8_t *data, uint8_t columns, uint8_t lines);

// Draw a chart element at the current cursor position...
void UC1701_drawColumn(uint8_t lines, uint8_t value);

#endif /* __UC1701_H__ */
