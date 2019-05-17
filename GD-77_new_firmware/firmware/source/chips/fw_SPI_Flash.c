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

#include <fw_SPI_Flash.h>

// private functions
static bool spi_flash_busy();
static void spi_flash_transfer_buf(uint8_t *inBuf,uint8_t *outBuf,int size);
static uint8_t spi_flash_transfer(uint8_t c);
static void spi_flash_setWriteEnable(bool cmd);
static void spi_flash_enable();
static void spi_flash_disable();

//COMMANDS. Not all implemented or used
#define W_EN 			0x06	//write enable
#define W_DE			0x04	//write disable
#define R_SR1			0x05	//read status reg 1
#define R_SR2			0x35	//read status reg 2
#define W_SR			0x01	//write status reg
#define PAGE_PGM		0x02	//page program
#define QPAGE_PGM		0x32	//quad input page program
#define BLK_E_64K		0xD8	//block erase 64KB
#define BLK_E_32K		0x52	//block erase 32KB
#define SECTOR_E		0x20	//sector erase 4KB
#define CHIP_ERASE		0xc7	//chip erase
#define CHIP_ERASE2		0x60	//=CHIP_ERASE
#define E_SUSPEND		0x75	//erase suspend
#define E_RESUME		0x7a	//erase resume
#define PDWN			0xb9	//power down
#define HIGH_PERF_M		0xa3	//high performance mode
#define CONT_R_RST		0xff	//continuous read mode reset
#define RELEASE			0xab	//release power down or HPM/Dev ID (deprecated)
#define R_MANUF_ID		0x90	//read Manufacturer and Dev ID (deprecated)
#define R_UNIQUE_ID		0x4b	//read unique ID (suggested)
#define R_JEDEC_ID		0x9f	//read JEDEC ID = Manuf+ID (suggested)
#define READ			0x03
#define FAST_READ		0x0b
#define SR1_BUSY_MASK	0x01
#define SR1_WEN_MASK	0x02
#define WINBOND_MANUF	0xef
  
bool SPI_Flash_init()
{
	int partNumber;
    PORT_SetPinMux(Port_SPI_FLASH_CS_U, Pin_SPI_FLASH_CS_U, kPORT_MuxAsGpio);//CS
    GPIO_PinInit(GPIO_SPI_FLASH_CS_U, Pin_SPI_FLASH_CS_U, &pin_config_output);

    PORT_SetPinMux(Port_SPI_FLASH_CLK_U, Pin_SPI_FLASH_CLK_U, kPORT_MuxAsGpio);//CLK
    GPIO_PinInit(GPIO_SPI_FLASH_CLK_U, Pin_SPI_FLASH_CLK_U, &pin_config_output);

    PORT_SetPinMux(Port_SPI_FLASH_DI_U, Pin_SPI_FLASH_DI_U, kPORT_MuxAsGpio);//Data out
    GPIO_PinInit(GPIO_SPI_FLASH_DI_U, Pin_SPI_FLASH_DI_U, &pin_config_input);

    PORT_SetPinMux(Port_SPI_FLASH_DO_U, Pin_SPI_FLASH_DO_U, kPORT_MuxAsGpio);//Data in
    GPIO_PinInit(GPIO_SPI_FLASH_DO_U, Pin_SPI_FLASH_DO_U, &pin_config_output);


    GPIO_PinWrite(GPIO_SPI_FLASH_CS_U, Pin_SPI_FLASH_CS_U, 1);// Disable
    GPIO_PinWrite(GPIO_SPI_FLASH_CLK_U, Pin_SPI_FLASH_CLK_U, 0);// Default clock pin to low

    partNumber = SPI_Flash_readPartID();

    if (partNumber == 0x4014 || partNumber == 0x4017)
    {
    	return true;
    }
    else
    {
    	return false;
    }
}

// Returns false for failed
// Note. There is no error checking that the device is not initially busy.
bool SPI_Flash_read(uint32_t addr,uint8_t *dataBuf,int size)
{
  uint8_t commandBuf[4]= {READ,addr>>16,addr>>8,addr} ;// command
  if(spi_flash_busy())
  {
    return false;
  }
  spi_flash_enable();
  spi_flash_transfer_buf(commandBuf,commandBuf,4);
  for(int i=0;i<size;i++)
  {
	  *dataBuf++ = spi_flash_transfer(0x00);
  }
  spi_flash_disable();
  return true;
}

int SPI_Flash_readStatusRegister()
{
  int r1,r2;

  spi_flash_enable();
  spi_flash_transfer(R_SR1);
  r1 = spi_flash_transfer(0xff);
  spi_flash_disable();
  spi_flash_enable();
  spi_flash_transfer(R_SR2);
  r2 = spi_flash_transfer(0xff);
  spi_flash_disable();
  return (((uint16_t)r2)<<8)|r1;
}

int SPI_Flash_readManufacturer()
{
  uint8_t commandBuf[4]= {R_JEDEC_ID,0x00,0x00,0x00} ;
  spi_flash_enable();
  spi_flash_transfer_buf(commandBuf,commandBuf,4);
  spi_flash_disable();
  return commandBuf[1];
}

int SPI_Flash_readPartID()
{
  uint8_t commandBuf[4]= {R_JEDEC_ID,0x00,0x00,0x00} ;
  spi_flash_enable();
  spi_flash_transfer_buf(commandBuf,commandBuf,4);
  spi_flash_disable();
  return (commandBuf[2]<<8)|commandBuf[3];
}

bool SPI_Flash_writePage(uint32_t addr_start,uint8_t *dataBuf)
{
	bool isBusy;
	int waitCounter = 5;// Worst case is something like 3mS
	uint8_t commandBuf[4]= {PAGE_PGM,addr_start>>16,addr_start>>8,0x00} ;

	spi_flash_setWriteEnable(true);

	spi_flash_enable();

	spi_flash_transfer_buf(commandBuf,commandBuf,4);// send the command and the address

	for(int i=0;i<0x100;i++)
	{
		spi_flash_transfer(*dataBuf++);
	}

	spi_flash_disable();

	do
	{
	    vTaskDelay(portTICK_PERIOD_MS * 1);
		isBusy = spi_flash_busy();
	} while (waitCounter-->0 && isBusy);

	return !isBusy;
}

// Returns true if erased and false if failed.
bool SPI_Flash_eraseSector(uint32_t addr_start)
{
	int waitCounter=500;// erase can take up to 500 mS
	bool isBusy;
	uint8_t commandBuf[4]= {SECTOR_E,addr_start>>16,addr_start>>8,0x00} ;
	spi_flash_enable();
	spi_flash_setWriteEnable(true);
	spi_flash_disable();

	spi_flash_enable();
	spi_flash_transfer_buf(commandBuf,commandBuf,4);
	spi_flash_disable();

	do
	{
	    vTaskDelay(portTICK_PERIOD_MS * 1);
		isBusy = spi_flash_busy();
	} while (waitCounter-->0 && isBusy);
	return !isBusy;// If still busy after
}

static void spi_flash_enable()
{
	GPIO_PinWrite(GPIO_SPI_FLASH_CS_U, Pin_SPI_FLASH_CS_U, 0);
}

static void spi_flash_disable()
{
	GPIO_PinWrite(GPIO_SPI_FLASH_CS_U, Pin_SPI_FLASH_CS_U, 1);
}

static uint8_t spi_flash_transfer(uint8_t c)
{
	for (uint8_t bit = 0; bit < 8; bit++)
	{
		GPIO_PinWrite(GPIO_SPI_FLASH_DO_U, Pin_SPI_FLASH_DO_U, (c & 0x80));// PinWrite treats all non zero values as a 1
		c <<= 1;
		c |= (GPIO_PinRead(GPIO_SPI_FLASH_DI_U, Pin_SPI_FLASH_DI_U) != 0);// read MISO
		// toggle the clock
		GPIO_PinWrite(GPIO_SPI_FLASH_CLK_U, Pin_SPI_FLASH_CLK_U, 1);
		GPIO_PinWrite(GPIO_SPI_FLASH_CLK_U, Pin_SPI_FLASH_CLK_U, 0);
	}
  return c;
}

static void spi_flash_transfer_buf(uint8_t *inBuf,uint8_t *outBuf,int size)
{
	while(size-->0)
	{
		*outBuf++ = spi_flash_transfer(*inBuf++);
	}
}

static bool spi_flash_busy()
{
  uint8_t r1;
  spi_flash_enable();
  spi_flash_transfer(R_SR1);
  r1 = spi_flash_transfer(0xff);
  spi_flash_disable();
  if(r1 & SR1_BUSY_MASK)
  {
    return true;
  }
  return false;
}

static void spi_flash_setWriteEnable(bool cmd)
{
	spi_flash_enable();
	spi_flash_transfer( cmd ? W_EN : W_DE );
	spi_flash_disable();
}
