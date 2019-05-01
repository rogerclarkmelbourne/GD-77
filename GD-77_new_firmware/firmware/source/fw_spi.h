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

#ifndef _FW_SPI_H_
#define _FW_SPI_H_

#include "fsl_port.h"
#include "fsl_dspi.h"
#include "fsl_dspi_freertos.h"

#define SPI_BAUDRATE (500000U) /*! Transfer baudrate - 500k */

#define SPI_DATA_LENGTH (256)         /*! Transfer size */
extern uint8_t spi_masterReceiveBuffer_SPI0[SPI_DATA_LENGTH];
extern uint8_t SPI_masterSendBuffer_SPI0[SPI_DATA_LENGTH];
extern uint8_t spi_masterReceiveBuffer_SPI1[SPI_DATA_LENGTH];
extern uint8_t SPI_masterSendBuffer_SPI1[SPI_DATA_LENGTH];

extern dspi_rtos_handle_t spi_master_rtos_handle_SPI0;
extern dspi_rtos_handle_t spi_master_rtos_handle_SPI1;

// SPI1 to C6000 (V_SPI)
// OUT/ON  B10 - SPI /V_CS to C6000
// OUT/OFF B11 - SPI V_CLK to C6000
// OUT/ON  B16 - SPI V_DI to C6000
// IN      B17 - SPI V_DO to C6000
#define Port_SPI_CS_C6000_V  PORTB
#define GPIO_SPI_CS_C6000_V  GPIOB
#define Pin_SPI_CS_C6000_V   10
#define Port_SPI_CLK_C6000_V PORTB
#define GPIO_SPI_CLK_C6000_V GPIOB
#define Pin_SPI_CLK_C6000_V  11
#define Port_SPI_DI_C6000_V  PORTB
#define GPIO_SPI_DI_C6000_V  GPIOB
#define Pin_SPI_DI_C6000_V   16
#define Port_SPI_DO_C6000_V  PORTB
#define GPIO_SPI_DO_C6000_V  GPIOB
#define Pin_SPI_DO_C6000_V   17

// SPI0 to C6000 (U_SPI)
// OUT/ON  D0 - SPI /U_CS to C6000
// OUT/OFF D1 - SPI U_CLK to C6000
// OUT/ON  D2 - SPI U_DI to C6000
// IN      D3 - SPI U_DO to C6000
#define Port_SPI_CS_C6000_U  PORTD
#define GPIO_SPI_CS_C6000_U  GPIOD
#define Pin_SPI_CS_C6000_U   0
#define Port_SPI_CLK_C6000_U PORTD
#define GPIO_SPI_CLK_C6000_U GPIOD
#define Pin_SPI_CLK_C6000_U  1
#define Port_SPI_DI_C6000_U  PORTD
#define GPIO_SPI_DI_C6000_U  GPIOD
#define Pin_SPI_DI_C6000_U   2
#define Port_SPI_DO_C6000_U  PORTD
#define GPIO_SPI_DO_C6000_U  GPIOD
#define Pin_SPI_DO_C6000_U   3

void init_SPI();
int setup_SPI0();
int setup_SPI1();

void clear_SPI_buffer_SPI0();
int write_SPI_page_reg_byte_SPI0(uint8_t page, uint8_t reg, uint8_t val);
int read_SPI_page_reg_byte_SPI0(uint8_t page, uint8_t reg, uint8_t* val);
int set_clear_SPI_page_reg_byte_with_mask_SPI0(uint8_t page, uint8_t reg, uint8_t mask, uint8_t val);
int write_SPI_page_reg_bytearray_SPI0(uint8_t page, uint8_t reg, uint8_t* values, uint8_t length);
int read_SPI_page_reg_bytearray_SPI0(uint8_t page, uint8_t reg, uint8_t* values, uint8_t length);

void clear_SPI_buffer_SPI1();
int write_SPI_page_reg_byte_SPI1(uint8_t page, uint8_t reg, uint8_t val);
int read_SPI_page_reg_byte_SPI1(uint8_t page, uint8_t reg, uint8_t* val);
int set_clear_SPI_page_reg_byte_with_mask_SPI1(uint8_t page, uint8_t reg, uint8_t mask, uint8_t val);
int write_SPI_page_reg_bytearray_SPI1(uint8_t page, uint8_t reg, uint8_t* values, uint8_t length);
int read_SPI_page_reg_bytearray_SPI1(uint8_t page, uint8_t reg, uint8_t* values, uint8_t length);

#endif /* _FW_SPI_H_ */
