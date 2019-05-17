/*
 * Copyright (C)2019 Roger Clark. VK3KYY / G4KYF
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

#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include "FreeRTOS.h"
#include "task.h"

#include "fw_common.h"

#define Port_SPI_FLASH_CS_U  	PORTA
#define GPIO_SPI_FLASH_CS_U  	GPIOA
#define Pin_SPI_FLASH_CS_U   	19

#define Port_SPI_FLASH_CLK_U 	PORTE
#define GPIO_SPI_FLASH_CLK_U 	GPIOE
#define Pin_SPI_FLASH_CLK_U  	5

#define Port_SPI_FLASH_DI_U  	PORTE
#define GPIO_SPI_FLASH_DI_U  	GPIOE
#define Pin_SPI_FLASH_DI_U   	6

#define Port_SPI_FLASH_DO_U  	PORTE
#define GPIO_SPI_FLASH_DO_U  	GPIOE
#define Pin_SPI_FLASH_DO_U   	4

// Public functions
bool SPI_Flash_init();
bool SPI_Flash_read(uint32_t addrress,uint8_t *buf,int size);
bool SPI_Flash_writePage(uint32_t address,uint8_t *dataBuf);// page is 256 bytes
bool SPI_Flash_eraseSector(uint32_t address);// sector is 16 pages  = 4k bytes
int SPI_Flash_readManufacturer();// Not necessarily Winbond !
int SPI_Flash_readPartID();// Should be 4014 for 1M or 4017 for 8M
int SPI_Flash_readStatusRegister();// May come in handy

#endif /* _SPI_FLASH_H_ */
