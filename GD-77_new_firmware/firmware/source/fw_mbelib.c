/*
 * Copyright (C) 2010 mbelib Author
 * GPG Key ID: 0xEA5EFE2C (9E7A 5527 9CDC EBF7 BF1B  D772 4F98 E863 EA5E FE2C)
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "fw_mbelib.h"

void mbe_checkGolayBlock (long int *block)
{
  static int i, syndrome, eccexpected, eccbits, databits;
  long int mask, block_l;

  block_l = *block;

  mask = 0x400000l;
  eccexpected = 0;
  for (i = 0; i < 12; i++)
    {
      if ((block_l & mask) != 0l)
        {
          eccexpected ^= golayGenerator[i];
        }
      mask = mask >> 1;
    }
  eccbits = (int) (block_l & 0x7ffl);
  syndrome = eccexpected ^ eccbits;

  databits = (int) (block_l >> 11);
  databits = databits ^ golayMatrix[syndrome];

  *block = (long) databits;
}

int mbe_golay2312 (char *in, char *out)
{

  int i, errs;
  long block;

  block = 0;
  for (i = 22; i >= 0; i--)
    {
      block = block << 1;
      block = block + in[i];
    }

  mbe_checkGolayBlock (&block);

  for (i = 22; i >= 11; i--)
    {
      out[i] = (block & 2048) >> 11;
      block = block << 1;
    }
  for (i = 10; i >= 0; i--)
    {
      out[i] = in[i];
    }

  errs = 0;
  for (i = 22; i >= 11; i--)
    {
      if (out[i] != in[i])
        {
          errs++;
        }
    }
  return (errs);
}

int mbe_eccAmbe3600x2450C0 (char ambe_fr[4][24])
{

  int j, errs;
  char in[23], out[23];

  for (j = 0; j < 23; j++)
    {
      in[j] = ambe_fr[0][j + 1];
    }
  errs = mbe_golay2312 (in, out);
  // ambe_fr[0][0] should be the C0 golay24 parity bit.
  // TODO: actually test that here...
  for (j = 0; j < 23; j++)
    {
      ambe_fr[0][j + 1] = out[j];
    }

  return (errs);
}

int mbe_eccAmbe3600x2450Data (char ambe_fr[4][24], char *ambe_d)
{

  int j, errs;
  char *ambe, gin[24], gout[24];

  ambe = ambe_d;
  // just copy C0
  for (j = 23; j > 11; j--)
    {
      *ambe = ambe_fr[0][j];
      ambe++;
    }

  // ecc and copy C1
  for (j = 0; j < 23; j++)
    {
      gin[j] = ambe_fr[1][j];
    }
  errs = mbe_golay2312 (gin, gout);
  for (j = 22; j > 10; j--)
    {
      *ambe = gout[j];
      ambe++;
    }

  // just copy C2
  for (j = 10; j >= 0; j--)
    {
      *ambe = ambe_fr[2][j];
      ambe++;
    }

  // just copy C3
  for (j = 13; j >= 0; j--)
    {
      *ambe = ambe_fr[3][j];
      ambe++;
    }

  return (errs);
}

void mbe_demodulateAmbe3600x2450Data (char ambe_fr[4][24])
{
  int i, j, k;
  unsigned short pr[115];
  unsigned short foo = 0;

  // create pseudo-random modulator
  for (i = 23; i >= 12; i--)
    {
      foo <<= 1;
      foo |= ambe_fr[0][i];
    }
  pr[0] = (16 * foo);
  for (i = 1; i < 24; i++)
    {
      pr[i] = (173 * pr[i - 1]) + 13849 - (65536 * (((173 * pr[i - 1]) + 13849) / 65536));
    }
  for (i = 1; i < 24; i++)
    {
      pr[i] = pr[i] / 32768;
    }

  // demodulate ambe_fr with pr
  k = 1;
  for (j = 22; j >= 0; j--)
    {
      ambe_fr[1][j] = ((ambe_fr[1][j]) ^ pr[k]);
      k++;
    }
}

void prepare_framedata(uint8_t *indata, char *ambe_d, int *errs, int *errs2)
{
	char ambe_fr[4][24];
	int *w, *x, *y, *z;
	uint8_t rampos = 0;

	w = (int*)rW;
	x = (int*)rX;
	y = (int*)rY;
	z = (int*)rZ;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ambe_fr[*w][*x] = (indata[rampos]>>(7-(2*j))) & 1;
			ambe_fr[*y][*z] = (indata[rampos]>>(7-(2*j+1))) & 1;
			w++;
			x++;
			y++;
			z++;
		}
		rampos++;
	}

    *errs = 0;
    *errs2 = 0;
    *errs = mbe_eccAmbe3600x2450C0 (ambe_fr);
    mbe_demodulateAmbe3600x2450Data (ambe_fr);
    *errs2 = *errs;
    *errs2 += mbe_eccAmbe3600x2450Data (ambe_fr, ambe_d);
}
