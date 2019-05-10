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

#ifndef _FW_I2C_H_
#define _FW_I2C_H_

#include "fsl_i2c.h"
#include "fsl_i2c_freertos.h"

#include "fw_common.h"

#define I2C_BAUDRATE (100000) /* 100K */
#define I2C_MASTER_SLAVE_ADDR_7BIT (0x71U)

#define I2C_DATA_LENGTH (32)  /* MAX is 256 */
extern uint8_t i2c_master_buff[I2C_DATA_LENGTH];

extern i2c_rtos_handle_t i2c_master_rtos_handle;

// I2C0a to AT24C512 EEPROM & AT1846S
// OUT/ON E24 - I2C SCL to AT24C512 EEPROM & AT1846S
// OUT/ON E25 - I2C SDA to AT24C512 EEPROM & AT1846S
#define Port_I2C0a_SCL     PORTE
#define GPIO_I2C0a_SCL     GPIOE
#define Pin_I2C0a_SCL	   24
#define Port_I2C0a_SDA     PORTE
#define GPIO_I2C0a_SDA     GPIOE
#define Pin_I2C0a_SDA	   25

// I2C0b to ALPU-MP-1413
// OUT/ON B2 - I2C SCL to ALPU-MP-1413
// OUT/ON B3 - I2C SDA to ALPU-MP-1413
#define Port_I2C0b_SCL     PORTB
#define GPIO_I2C0b_SCL     GPIOB
#define Pin_I2C0b_SCL	   2
#define Port_I2C0b_SDA     PORTB
#define GPIO_I2C0b_SDA     GPIOB
#define Pin_I2C0b_SDA	   3

void init_I2C0a();
void init_I2C0b();
int setup_I2C0();

void clear_I2C_buffer();
int write_I2C_reg_2byte(uint8_t addr, uint8_t reg, uint8_t val1, uint8_t val2);
int read_I2C_reg_2byte(uint8_t addr, uint8_t reg, uint8_t* val1, uint8_t* val2);
int set_clear_I2C_reg_2byte_with_mask(uint8_t reg, uint8_t mask1, uint8_t mask2, uint8_t val1, uint8_t val2);

#endif /* _FW_I2C_H_ */
