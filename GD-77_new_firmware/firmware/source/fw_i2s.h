/*
 * Copyright (C)2019 Kai Ludwig, DG4KLU
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

#ifndef _FW_I2S_H_
#define _FW_I2S_H_

#include "fsl_port.h"
#include "fsl_dmamux.h"
#include "fsl_edma.h"
#include "fsl_sai.h"
#include "fsl_sai_edma.h"

// I2S to C6000 (I2S)
// OUT/ON  A16 - I2S FS to C6000
// OUT/OFF A14 - I2S CK to C6000
// OUT/ON  A12 - I2S RX to C6000
// IN      A15 - I2S TX to C6000
#define Port_I2S_FS_C6000    PORTA
#define GPIO_I2S_FS_C6000    GPIOA
#define Pin_I2S_FS_C6000     16
#define Port_I2S_CK_C6000    PORTA
#define GPIO_I2S_CK_C6000    GPIOA
#define Pin_I2S_CK_C6000     14
#define Port_I2S_RX_C6000    PORTA
#define GPIO_I2S_RX_C6000    GPIOA
#define Pin_I2S_RX_C6000     12
#define Port_I2S_TX_C6000    PORTA
#define GPIO_I2S_TX_C6000    GPIOA
#define Pin_I2S_TX_C6000     15

void init_I2S();
void setup_I2S();

#endif /* _FW_I2S_H_ */
