/*
 * File extractor to create supplemental data (*.dat files) needed for building the custom GD-77 firmware.
 * 
 * Copyright (C)2019 Roger Clark. VK3KYY
 *
 * GD-77 firmware decrypter/encrypter by DG4KLU.
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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.IO;
using System.Net.Security;
using System.Security.Cryptography.X509Certificates;
using System.Threading;

namespace codec_dat_files_creator
{
	class Program
	{
		static void Main(string[] args)
		{
			try
			{
				// These ServicePointManager settings seem to be necessary as github appears to switch to SSL even if the request is non SSL
				ServicePointManager.Expect100Continue = true;
				ServicePointManager.SecurityProtocol = SecurityProtocolType.Tls12;

				WebClient wc = new WebClient();

				Console.WriteLine("Download encrypted firmware (sgl) file from GitHub.\nPlease wait.\nThis may take some time ;-) ");
				byte[] dataBuf = wc.DownloadData("http://github.com/rogerclarkmelbourne/Radioddity_GD-77/raw/master/firmware/GD-77_V3.1.1.sgl");
				Console.WriteLine("sgl file has been downloaded.");
				Thread.Sleep(500);
				
				dataBuf = checkForSGLAndReturnEncryptedData(dataBuf);
				if (dataBuf != null)
				{
					Console.WriteLine("sgl file is valid.");
					Console.WriteLine("decrypting .....");
					dataBuf = decodeFwV311data(dataBuf);
					Console.WriteLine("File has been decrypted");

					byte[] fileBuf = new byte[0x4000 - 0x400];
					Buffer.BlockCopy(dataBuf, 0x400, fileBuf, 0, fileBuf.Length);
					File.WriteAllBytes("fw_311_0x00004400_0x00007fff.dat", fileBuf);
					Console.WriteLine("Writing fw_311_0x00004400_0x00007fff.dat");

					fileBuf = new byte[0x77000 - 0x4c000];
					Buffer.BlockCopy(dataBuf, 0x4c000, fileBuf, 0, fileBuf.Length);
					File.WriteAllBytes("fw_311_0x00050000_0x0007afff.dat", fileBuf);
					Console.WriteLine("Writing fw_311_0x00050000_0x0007afff.dat");

					Console.WriteLine("Finished...");
                    Console.WriteLine("Press return to exit");
                    Console.ReadLine();
                }
                else
				{
					Console.WriteLine("Error. sgl file contains invalid header :-(");
					Console.WriteLine("Press return to exit");
					Console.ReadLine();
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error :-(  "+ex.ToString());
				Console.WriteLine("Press return to exit");
				Console.ReadLine();
			}
		}

		/// <summary>
		/// Certificate validation callback.
		/// </summary>
		private static bool ValidateRemoteCertificate(object sender, X509Certificate cert, X509Chain chain, SslPolicyErrors error)
		{
			// If the certificate is a valid, signed certificate, return true.
			if (error == System.Net.Security.SslPolicyErrors.None)
			{
				return true;
			}

			Console.WriteLine("X509Certificate [{0}] Policy Error: '{1}'",
				cert.Subject,
				error.ToString());

			return false;
		}

		static byte[] decodeFwV311data(byte[] sglData)
		{
			int data=0;
			int shift = 0;// The decrytion table matches firmware 3.1.1 by default hence the initial shift is zero
			for (int i = 0; i < sglData.Length; i++)
			{
				data = ~(((sglData[i] << 3) & 0xF8) | ((sglData[i] >> 5) & 0x07));
				sglData[i] = (byte)(data ^ DataArrays.DecryptionTable[shift++]);
				if (shift >= 0x7fff)
				{
					shift = 0;
				}
			}
			return sglData;
		}

		static byte[] checkForSGLAndReturnEncryptedData(byte[] fileBuf)
		{
			byte[] header_tag = new byte[] { (byte)'S', (byte)'G', (byte)'L', (byte)'!' };

			// read header tag
			byte[] buf_in_4 = new byte[4];
			Buffer.BlockCopy(fileBuf, 0, buf_in_4, 0, buf_in_4.Length);

			if (buf_in_4.SequenceEqual(header_tag))
			{
				// read and decode offset and xor tag
				Buffer.BlockCopy(fileBuf, 0x000C, buf_in_4, 0, buf_in_4.Length);
				for (int i = 0; i < buf_in_4.Length; i++)
				{
					buf_in_4[i] = (byte)(buf_in_4[i] ^ header_tag[i]);
				}
				int offset = buf_in_4[0] + 256 * buf_in_4[1];
				byte[] xor_data = new byte[] { buf_in_4[2], buf_in_4[3] };

				// read and decode part of the header
				byte[] buf_in_512 = new byte[512];
				Buffer.BlockCopy(fileBuf, offset + 0x0006, buf_in_512, 0, buf_in_512.Length);
				int xor_idx = 0;
				for (int i = 0; i < buf_in_512.Length; i++)
				{
					buf_in_512[i] = (byte)(buf_in_512[i] ^ xor_data[xor_idx]);
					xor_idx++;
					if (xor_idx == 2)
					{
						xor_idx = 0;
					}
				}

                // extract length
                byte length1 = (byte)buf_in_512[0x0000];
				byte length2 = (byte)buf_in_512[0x0001];
				byte length3 = (byte)buf_in_512[0x0002];
				byte length4 = (byte)buf_in_512[0x0003];
				int length = (length4 << 24) + (length3 << 16) + (length2 << 8) + length1;

                byte[] retBuf = new byte[length];
				Buffer.BlockCopy(fileBuf, fileBuf.Length - length, retBuf, 0, retBuf.Length);
				return retBuf;
			}
			else
			{
				Console.WriteLine("ERROR: SGL! header missing.");
				return null;
			}
		}
	}
}
