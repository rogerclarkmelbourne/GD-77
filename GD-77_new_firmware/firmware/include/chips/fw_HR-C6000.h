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

#ifndef _FW_HR_C6000_H_
#define _FW_HR_C6000_H_

#include "FreeRTOS.h"
#include "task.h"

#include "fw_spi.h"
#include "fw_usb_com.h"

#include "fw_codec.h"

// C6000 interrupts
// IN      C7  - C6000 RF_RX_INTER
// IN      C16 - C6000 RF_TX_INTER
// IN      C17 - C6000 SYS_INTER
// IN      C18 - C6000 TIME_SLOT_INTER
#define Port_INT_C6000_RF_RX PORTC
#define GPIO_INT_C6000_RF_RX GPIOC
#define Pin_INT_C6000_RF_RX  7
#define Port_INT_C6000_RF_TX PORTC
#define GPIO_INT_C6000_RF_TX GPIOC
#define Pin_INT_C6000_RF_TX  16
#define Port_INT_C6000_SYS   PORTC
#define GPIO_INT_C6000_SYS   GPIOC
#define Pin_INT_C6000_SYS    17
#define Port_INT_C6000_TS    PORTC
#define GPIO_INT_C6000_TS    GPIOC
#define Pin_INT_C6000_TS     18

// Connections with C6000
// OUT/ON  E0 - C6000 RESETn
// OUT/ON  E1 - C6000 PWD
#define Port_INT_C6000_RESET PORTE
#define GPIO_INT_C6000_RESET GPIOE
#define Pin_INT_C6000_RESET  0
#define Port_INT_C6000_PWD   PORTE
#define GPIO_INT_C6000_PWD   GPIOE
#define Pin_INT_C6000_PWD    1

extern bool int_sys;
extern bool int_ts;

extern int slot_state;
extern int tick_cnt;
extern int skip_count;

void SPI_HR_C6000_init();
void SPI_C6000_postinit1();
void SPI_C6000_postinit2();
void SPI_C6000_postinit3a();
void SPI_C6000_postinit3b();
void PORTC_IRQHandler(void);
void init_HR_C6000_interrupts();
void init_digital();
void terminate_digital();
void tick_HR_C6000();

#endif /* _FW_HR_C6000_H_ */
