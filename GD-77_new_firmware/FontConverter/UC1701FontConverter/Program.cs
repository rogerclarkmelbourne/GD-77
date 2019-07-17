/*
 * Copyright (C)2019 	Roger Clark, VK3KYY / G4KYF
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace UC1701FontConverter
{
	class Program
	{
		static void Main(string[] args)
		{
			byte [] inBytes;
			byte [] outBytes;
			int charWidthBytes=1;
			int charHeightBytes=2;
			byte[] transposedBytes;
			byte[] inTmp = new byte[16];
			string inFileName="";

			if (args.Length > 0)
			{
				inFileName = args[0];
			}
			else
			{
				Console.WriteLine("Usage: UC1701FontConverter filename");
				Console.ReadLine();
				return;
			}

			inBytes = File.ReadAllBytes(inFileName);
			if (args.Length == 3)
			{
				charWidthBytes = Int32.Parse(args[1]);
				charHeightBytes = Int32.Parse(args[2]);
			}
			
			outBytes = new byte[inBytes.Length+8];
			outBytes[2] = 32;// first character code
			outBytes[3] = 126;// last char code
			outBytes[4] = (byte)(charWidthBytes * 8);// width of the character in pixels
			outBytes[5] = (byte)(charHeightBytes * 8);// height of the character in pixels
			outBytes[6] = 1;// page height per character ??
			outBytes[7] = 16;// bytes per character (normally width * height /8)
		

			int charLengthBytes = (charWidthBytes * 8 * charHeightBytes);
			int numCharsInFont =  inBytes.Length / charLengthBytes;

			for (int inChar = 0; inChar < numCharsInFont; inChar++)
			{
				Buffer.BlockCopy(inBytes, inChar * charLengthBytes, inTmp, 0, 8);//+ charLengthBytes / 2
				transposedBytes = transpose8(inTmp);
				Buffer.BlockCopy(transposedBytes, 0, outBytes, inChar * charLengthBytes , 8);

				Buffer.BlockCopy(inBytes, inChar * charLengthBytes + charLengthBytes / 2, inTmp, 0, 8);
				transposedBytes = transpose8(inTmp);
				Buffer.BlockCopy(transposedBytes, 0, outBytes, inChar * charLengthBytes + charLengthBytes / 2, 8);
			}

			string	cStr=	"const uint8_t font_" + Path.GetFileNameWithoutExtension(inFileName) + "[] = {\n" +
							"0x00,00,\n" +
							"32,// first char code\n" +
							"127,// last char code\n" +
							"8,// width of the character in pixels\n" +
							"16,// height of the character in pixels\n" +
							"1,// page height per character ??\n" +
							"16,// bytes per character (normally width * height /8)\n";

			for (int i = 0; i < outBytes.Length - 8; i++)
			{
				cStr += "0x" + outBytes[i].ToString("X2");
				if (i< outBytes.Length-9)
				{
					cStr += ",";
				}
				if (i != 0 && i % 16 == 15)
				{
					cStr += "\n";
				}
			}
			cStr += "};";

			File.WriteAllText(Path.GetFileNameWithoutExtension(inFileName) + ".h", cStr);
		//	File.WriteAllBytes(Path.GetFileNameWithoutExtension(inFileName) + "_converted." + Path.GetExtension(inFileName), outBytes);
			
		}
		/* not needed
		static byte reverseBitOrder(byte b) 
		{
		   b = (byte)((b & 0xF0) >> 4 | (b & 0x0F) << 4);
		   b = (byte)((b & 0xCC) >> 2 | (b & 0x33) << 2);
		   b = (byte)((b & 0xAA) >> 1 | (b & 0x55) << 1);
		   return b;
		}*/

		static byte [] transpose8(byte[] inBytes)
		{
			int i,j;
			byte [] outBytes = new byte[8];

			// rotates the bits within the 8 bytes as if its a 8 x 8 pixel matrix.
			for(i= 0; i < 8; i++) 
			{
				for(j = 0; j < 8; j++) 
				{
					outBytes[i] = (byte) ((outBytes[i] >> 1) |  ((inBytes[j] >> (7 - i)) & 0x01)<<7);// rotate and also reverse the bit order 
				}
				//outBytes[i] = reverseBitOrder(outBytes[i]);// Don't need to reverse the bit order, as its now done in the line above.
			}
			return outBytes;
		}
	}
}