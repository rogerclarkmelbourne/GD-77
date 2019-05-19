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

#include "fw_display.h"

#include "UC1701.h"
#include "UC1701_charset.h"

// The size of the display, in pixels...
uint8_t UC1701_width;
uint8_t UC1701_height;

// Current cursor position...
uint8_t UC1701_column;
uint8_t UC1701_line;

// User-defined glyphs (below the ASCII space character)...
const uint8_t *UC1701_custom[' '];

void UC1701_transfer(uint8_t mode, int data1)
{
   GPIO_PinWrite(GPIO_Display_RS, Pin_Display_RS, mode);
   for (int i=0; i<8; i++)
   {
	 GPIO_PinWrite(GPIO_Display_SCK, Pin_Display_SCK, 0);
	 GPIO_PinWrite(GPIO_Display_SDA, Pin_Display_SDA, data1&0x80);
	 GPIO_PinWrite(GPIO_Display_SCK, Pin_Display_SCK, 1);
	 data1=data1<<1;
   }
}

void UC1701_begin()
{
	UC1701_width = 128;
	UC1701_height = 64;

	UC1701_column = 0;
	UC1701_line = 0;

	GPIO_PinWrite(GPIO_Display_CS, Pin_Display_CS, 0);// Enable CS permanently

    // Set the LCD parameters...
    UC1701_transfer(UC1701_COMMAND, 0xE2); // System Reset
    UC1701_transfer(UC1701_COMMAND, 0x2F); // Voltage Follower On
    UC1701_transfer(UC1701_COMMAND, 0x81); // Set Electronic Volume = 15
    UC1701_transfer(UC1701_COMMAND, 0x0E); //
    UC1701_transfer(UC1701_COMMAND, 0xA2); // Set Bias = 1/9
    UC1701_transfer(UC1701_COMMAND, 0xA1); // Set SEG Direction
    UC1701_transfer(UC1701_COMMAND, 0xC0); // Set COM Direction
    UC1701_transfer(UC1701_COMMAND, 0xA4); // Normal display
    UC1701_clear();
    UC1701_transfer(UC1701_COMMAND, 0xAF); // Set Display Enable
}

void UC1701_clear()
{
	for  (uint8_t j = 0; j < 8; j++)
	{
		UC1701_setCursor(0, j);
		for (uint8_t i = 0; i < 132 ; i++)
		{
			UC1701_transfer(UC1701_DATA, 0x00);
		}
	}

	UC1701_setCursor(0, 0);
}

void UC1701_setCursor(uint8_t column, uint8_t line)
{
       int i, j;
       column = column+4;
       UC1701_column = column;
       UC1701_line = line;

       i=(column&0xF0)>>4;
       j=column&0x0F;
       UC1701_transfer(UC1701_COMMAND, 0xb0+line);
       UC1701_transfer(UC1701_COMMAND, 0x10+i);
       UC1701_transfer(UC1701_COMMAND, j);
}

void UC1701_printCentered(uint8_t y, char *text)
{
	UC1701_setCursor(66-3*strlen(text),y);
	UC1701_print(text);
}

void UC1701_printAt(uint8_t x, uint8_t y, char *text)
{
	UC1701_setCursor(x,y);
	UC1701_print(text);
}

void UC1701_print(char *text)
{
	int i=0;
	while (text[i]!=0)
	{
		UC1701_write(text[i]);
		i++;
	}
}

int UC1701_write(uint8_t chr)
{
    const uint8_t *glyph;
    uint8_t pgm_buffer[5];

    // ASCII 7-bit only...
    if (chr >= 0x80)
    {
        return 0;
    }

    if (chr == '\r')
    {
    	UC1701_setCursor(0, UC1701_line);
        return 1;
    }
    else
    {
    	if (chr == '\n')
    	{
        	UC1701_setCursor(UC1701_column, UC1701_line + 1);
            return 1;
    	}
    }

    if (chr >= ' ')
    {
        // Regular ASCII characters are kept in flash to save RAM...
        memcpy(pgm_buffer, &UC1701_charset[chr - ' '], sizeof(pgm_buffer));
        glyph = pgm_buffer;
    }
    else
    {
        // Custom glyphs, on the other hand, are stored in RAM...
        if (UC1701_custom[chr])
        {
            glyph = UC1701_custom[chr];
        }
        else
        {
            // Default to a space character if unset...
            memcpy(pgm_buffer, &UC1701_charset[0], sizeof(pgm_buffer));
            glyph = pgm_buffer;
        }
    }

    // Output one column at a time...
    for (uint8_t i = 0; i < 5; i++)
    {
    	UC1701_transfer(UC1701_DATA, glyph[i]);
    }

    // One column between characters...
    UC1701_transfer(UC1701_DATA, 0x00);

    // Update the cursor position...
    UC1701_column = (UC1701_column + 6) % UC1701_width;

    if (UC1701_column == 0)
    {
    	UC1701_line = (UC1701_line + 1) % (UC1701_height/9 + 1);
    }

    return 1;
}

void UC1701_createChar(uint8_t chr, const uint8_t *glyph)
{
    // ASCII 0-31 only...
    if (chr >= ' ')
    {
        return;
    }
    
    UC1701_custom[chr] = glyph;
}

void UC1701_clearLine()
{
	UC1701_setCursor(0, UC1701_line);

    for (uint8_t i = 4; i < 132; i++)
    {
    	UC1701_transfer(UC1701_DATA, 0x00);
    }

    UC1701_setCursor(0, UC1701_line);
}

void UC1701_home()
{
	UC1701_setCursor(0, UC1701_line);
}

void UC1701_drawBitmap(const uint8_t *data, uint8_t columns, uint8_t lines)
{
	uint8_t scolumn = UC1701_column;
	uint8_t sline = UC1701_line;

    // The bitmap will be clipped at the right/bottom edge of the display...
	uint8_t mx = (scolumn + columns > UC1701_width) ? (UC1701_width - scolumn) : columns;
	uint8_t my = (sline + lines > UC1701_height/8) ? (UC1701_height/8 - sline) : lines;

    for (uint8_t y = 0; y < my; y++)
    {
    	UC1701_setCursor(scolumn, sline + y);

        for (uint8_t x = 0; x < mx; x++)
        {
        	UC1701_transfer(UC1701_DATA, data[y * columns + x]);
        }
    }

    // Leave the cursor in a consistent position...
    UC1701_setCursor(scolumn + columns, sline);
}

void UC1701_drawColumn(uint8_t lines, uint8_t value)
{
	uint8_t scolumn = UC1701_column;
	uint8_t sline = UC1701_line;

    // Keep "value" within range...
    if (value > lines*8)
    {
        value = lines*8;
    }

    // Find the line where "value" resides...
    uint8_t mark = (lines*8 - 1 - value)/8;
    
    // Clear the lines above the mark...
    for (uint8_t line = 0; line < mark; line++)
    {
    	UC1701_setCursor(scolumn, sline + line);
    	UC1701_transfer(UC1701_DATA, 0x00);
    }

    // Compute the byte to draw at the "mark" line...
    uint8_t b = 0xff;
    for (uint8_t i = 0; i < lines*8 - mark*8 - value; i++)
    {
        b <<= 1;
    }

    UC1701_setCursor(scolumn, sline + mark);
    UC1701_transfer(UC1701_DATA, b);

    // Fill the lines below the mark...
    for (uint8_t line = mark + 1; line < lines; line++)
    {
    	UC1701_setCursor(scolumn, sline + line);
    	UC1701_transfer(UC1701_DATA, 0xff);
    }
  
    // Leave the cursor in a consistent position...
    UC1701_setCursor(scolumn + 1, sline);
}
