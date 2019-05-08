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

#ifndef _FW_MBELIB_H_
#define _FW_MBELIB_H_

#include "fw_common.h"

void mbe_checkGolayBlock (long int *block);
int mbe_golay2312 (char *in, char *out);
int mbe_eccAmbe3600x2450C0 (char ambe_fr[4][24]);
int mbe_eccAmbe3600x2450Data (char ambe_fr[4][24], char *ambe_d);
void mbe_demodulateAmbe3600x2450Data (char ambe_fr[4][24]);
void prepare_framedata(uint8_t *indata, char *ambe_d, int *errs, int *errs2);

#endif /* _FW_MBELIB_H_ */
