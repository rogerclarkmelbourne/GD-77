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

#include "fw_spi.h"

uint8_t spi_masterReceiveBuffer_SPI0[SPI_DATA_LENGTH] = {0};
uint8_t SPI_masterSendBuffer_SPI0[SPI_DATA_LENGTH] = {0};
uint8_t spi_masterReceiveBuffer_SPI1[SPI_DATA_LENGTH] = {0};
uint8_t SPI_masterSendBuffer_SPI1[SPI_DATA_LENGTH] = {0};

dspi_rtos_handle_t spi_master_rtos_handle_SPI0;
dspi_rtos_handle_t spi_master_rtos_handle_SPI1;

void init_SPI()
{
    /* PORTD0 is configured as SPI0_CS0 */
    PORT_SetPinMux(Port_SPI_CS_C6000_U, Pin_SPI_CS_C6000_U, kPORT_MuxAlt2);

    /* PORTD1 is configured as SPI0_SCK */
    PORT_SetPinMux(Port_SPI_CLK_C6000_U, Pin_SPI_CLK_C6000_U, kPORT_MuxAlt2);

    /* PORTD2 is configured as SPI0_SOUT */
    PORT_SetPinMux(Port_SPI_DI_C6000_U, Pin_SPI_DI_C6000_U, kPORT_MuxAlt2);

    /* PORTD3 is configured as SPI0_SIN */
    PORT_SetPinMux(Port_SPI_DO_C6000_U, Pin_SPI_DO_C6000_U, kPORT_MuxAlt2);

    /* PORTB10 is configured as SPI1_CS0 */
    PORT_SetPinMux(Port_SPI_CS_C6000_V, Pin_SPI_CS_C6000_V, kPORT_MuxAlt2);

    /* PORTB11 is configured as SPI1_SCK */
    PORT_SetPinMux(Port_SPI_CLK_C6000_V, Pin_SPI_CLK_C6000_V, kPORT_MuxAlt2);

    /* PORTB16 is configured as SPI1_SOUT */
    PORT_SetPinMux(Port_SPI_DI_C6000_V, Pin_SPI_DI_C6000_V, kPORT_MuxAlt2);

    /* PORTB17 is configured as SPI1_SIN */
    PORT_SetPinMux(Port_SPI_DO_C6000_V, Pin_SPI_DO_C6000_V, kPORT_MuxAlt2);

    NVIC_SetPriority(SPI0_IRQn, 3);
    NVIC_SetPriority(SPI1_IRQn, 3);
}

int setup_SPI0()
{
    dspi_master_config_t masterConfig_SPI0;
    status_t status;

	/*Master config*/
	masterConfig_SPI0.whichCtar = kDSPI_Ctar0;
	masterConfig_SPI0.ctarConfig.baudRate = SPI_BAUDRATE;
	masterConfig_SPI0.ctarConfig.bitsPerFrame = 8;
	masterConfig_SPI0.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
	masterConfig_SPI0.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
	masterConfig_SPI0.ctarConfig.direction = kDSPI_MsbFirst;
	masterConfig_SPI0.ctarConfig.pcsToSckDelayInNanoSec = 2000;
	masterConfig_SPI0.ctarConfig.lastSckToPcsDelayInNanoSec = 2000;
	masterConfig_SPI0.ctarConfig.betweenTransferDelayInNanoSec = 1000;

	masterConfig_SPI0.whichPcs = kDSPI_Pcs0;
	masterConfig_SPI0.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

	masterConfig_SPI0.enableContinuousSCK = false;
	masterConfig_SPI0.enableRxFifoOverWrite = false;
	masterConfig_SPI0.enableModifiedTimingFormat = false;
	masterConfig_SPI0.samplePoint = kDSPI_SckToSin0Clock;

	status = DSPI_RTOS_Init(&spi_master_rtos_handle_SPI0, SPI0, &masterConfig_SPI0, CLOCK_GetFreq(DSPI0_CLK_SRC));
	if (status != kStatus_Success)
	{
    	return status;
	}

	return kStatus_Success;
}

int setup_SPI1()
{
    dspi_master_config_t masterConfig_SPI1;
    status_t status;

	/*Master config*/
    masterConfig_SPI1.whichCtar = kDSPI_Ctar0;
    masterConfig_SPI1.ctarConfig.baudRate = SPI_BAUDRATE;
    masterConfig_SPI1.ctarConfig.bitsPerFrame = 8;
    masterConfig_SPI1.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
    masterConfig_SPI1.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
    masterConfig_SPI1.ctarConfig.direction = kDSPI_MsbFirst;
    masterConfig_SPI1.ctarConfig.pcsToSckDelayInNanoSec = 2000;
    masterConfig_SPI1.ctarConfig.lastSckToPcsDelayInNanoSec = 2000;
    masterConfig_SPI1.ctarConfig.betweenTransferDelayInNanoSec = 1000;

    masterConfig_SPI1.whichPcs = kDSPI_Pcs0;
    masterConfig_SPI1.pcsActiveHighOrLow = kDSPI_PcsActiveLow;

    masterConfig_SPI1.enableContinuousSCK = false;
    masterConfig_SPI1.enableRxFifoOverWrite = false;
    masterConfig_SPI1.enableModifiedTimingFormat = false;
    masterConfig_SPI1.samplePoint = kDSPI_SckToSin0Clock;

	status = DSPI_RTOS_Init(&spi_master_rtos_handle_SPI1, SPI1, &masterConfig_SPI1, CLOCK_GetFreq(DSPI1_CLK_SRC));
	if (status != kStatus_Success)
	{
    	return status;
	}

	return kStatus_Success;
}

void clear_SPI_buffer_SPI0()
{
    for (uint32_t i = 0; i < SPI_DATA_LENGTH; i++)
    {
        SPI_masterSendBuffer_SPI0[i] = 0;
        spi_masterReceiveBuffer_SPI0[i] = 0;
    }
}

int write_SPI_page_reg_byte_SPI0(uint8_t page, uint8_t reg, uint8_t val)
{
    dspi_transfer_t masterXfer;
    status_t status;

	clear_SPI_buffer_SPI0();
	SPI_masterSendBuffer_SPI0[0]=page;
	SPI_masterSendBuffer_SPI0[1]=reg;
	SPI_masterSendBuffer_SPI0[2]=val;

    /*Start master transfer*/
    masterXfer.txData = SPI_masterSendBuffer_SPI0;
    masterXfer.rxData = spi_masterReceiveBuffer_SPI0;
    masterXfer.dataSize = 3;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

    status = DSPI_RTOS_Transfer(&spi_master_rtos_handle_SPI0, &masterXfer);
    if (status != kStatus_Success)
    {
    	return status;
    }

	return kStatus_Success;
}

int read_SPI_page_reg_byte_SPI0(uint8_t page, uint8_t reg, uint8_t* val)
{
    dspi_transfer_t masterXfer;
    status_t status;

	clear_SPI_buffer_SPI0();
	SPI_masterSendBuffer_SPI0[0]=page | 0x80;
	SPI_masterSendBuffer_SPI0[1]=reg;
	SPI_masterSendBuffer_SPI0[2]=0xFF;

    /*Start master transfer*/
    masterXfer.txData = SPI_masterSendBuffer_SPI0;
    masterXfer.rxData = spi_masterReceiveBuffer_SPI0;
    masterXfer.dataSize = 3;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

    status = DSPI_RTOS_Transfer(&spi_master_rtos_handle_SPI0, &masterXfer);
    if (status != kStatus_Success)
    {
    	return status;
    }

	*val=spi_masterReceiveBuffer_SPI0[2];

	return kStatus_Success;
}

int set_clear_SPI_page_reg_byte_with_mask_SPI0(uint8_t page, uint8_t reg, uint8_t mask, uint8_t val)
{
    status_t status;

	uint8_t tmp_val;
	status = read_SPI_page_reg_byte_SPI0(page, reg, &tmp_val);
    if (status != kStatus_Success)
    {
    	return status;
    }
	tmp_val=val | (tmp_val & mask);
	status = write_SPI_page_reg_byte_SPI0(page, reg, tmp_val);
    if (status != kStatus_Success)
    {
    	return status;
    }

	return kStatus_Success;
}

int write_SPI_page_reg_bytearray_SPI0(uint8_t page, uint8_t reg, uint8_t* values, uint8_t length)
{
    dspi_transfer_t masterXfer;
    status_t status;

	clear_SPI_buffer_SPI0();
	SPI_masterSendBuffer_SPI0[0]=page;
	SPI_masterSendBuffer_SPI0[1]=reg;
	for (int i=0; i<length; i++)
	{
		SPI_masterSendBuffer_SPI0[i+2]=values[i];
	}

    /*Start master transfer*/
    masterXfer.txData = SPI_masterSendBuffer_SPI0;
    masterXfer.rxData = spi_masterReceiveBuffer_SPI0;
    masterXfer.dataSize = length+2;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

    status = DSPI_RTOS_Transfer(&spi_master_rtos_handle_SPI0, &masterXfer);
    if (status != kStatus_Success)
    {
    	return status;
    }

	return kStatus_Success;
}

int read_SPI_page_reg_bytearray_SPI0(uint8_t page, uint8_t reg, uint8_t* values, uint8_t length)
{
    dspi_transfer_t masterXfer;
    status_t status;

	clear_SPI_buffer_SPI0();
	SPI_masterSendBuffer_SPI0[0]=page | 0x80;
	SPI_masterSendBuffer_SPI0[1]=reg;
	for (int i=0; i<length; i++)
	{
		SPI_masterSendBuffer_SPI0[i+2]=0xFF;
	}

    /*Start master transfer*/
    masterXfer.txData = SPI_masterSendBuffer_SPI0;
    masterXfer.rxData = spi_masterReceiveBuffer_SPI0;
    masterXfer.dataSize = length+2;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

    status = DSPI_RTOS_Transfer(&spi_master_rtos_handle_SPI0, &masterXfer);
    if (status != kStatus_Success)
    {
    	return status;
    }

	for (int i=0; i<length; i++)
	{
		values[i]=spi_masterReceiveBuffer_SPI0[i+2];
	}

	return kStatus_Success;
}

void clear_SPI_buffer_SPI1()
{
    for (uint32_t i = 0; i < SPI_DATA_LENGTH; i++)
    {
        SPI_masterSendBuffer_SPI1[i] = 0;
        spi_masterReceiveBuffer_SPI1[i] = 0;
    }
}

int write_SPI_page_reg_byte_SPI1(uint8_t page, uint8_t reg, uint8_t val)
{
    dspi_transfer_t masterXfer;
    status_t status;

	clear_SPI_buffer_SPI1();
	SPI_masterSendBuffer_SPI1[0]=page;
	SPI_masterSendBuffer_SPI1[1]=reg;
	SPI_masterSendBuffer_SPI1[2]=val;

    /*Start master transfer*/
    masterXfer.txData = SPI_masterSendBuffer_SPI1;
    masterXfer.rxData = spi_masterReceiveBuffer_SPI1;
    masterXfer.dataSize = 3;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

    status = DSPI_RTOS_Transfer(&spi_master_rtos_handle_SPI1, &masterXfer);
    if (status != kStatus_Success)
    {
    	return status;
    }

	return kStatus_Success;
}

int read_SPI_page_reg_byte_SPI1(uint8_t page, uint8_t reg, uint8_t* val)
{
    dspi_transfer_t masterXfer;
    status_t status;

	clear_SPI_buffer_SPI1();
	SPI_masterSendBuffer_SPI1[0]=page | 0x80;
	SPI_masterSendBuffer_SPI1[1]=reg;
	SPI_masterSendBuffer_SPI1[2]=0xFF;

    /*Start master transfer*/
    masterXfer.txData = SPI_masterSendBuffer_SPI1;
    masterXfer.rxData = spi_masterReceiveBuffer_SPI1;
    masterXfer.dataSize = 3;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

    status = DSPI_RTOS_Transfer(&spi_master_rtos_handle_SPI1, &masterXfer);
    if (status != kStatus_Success)
    {
    	return status;
    }

	*val=spi_masterReceiveBuffer_SPI1[2];

	return kStatus_Success;
}

int set_clear_SPI_page_reg_byte_with_mask_SPI1(uint8_t page, uint8_t reg, uint8_t mask, uint8_t val)
{
    status_t status;

	uint8_t tmp_val;
	status = read_SPI_page_reg_byte_SPI1(page, reg, &tmp_val);
    if (status != kStatus_Success)
    {
    	return status;
    }
	tmp_val=val | (tmp_val & mask);
	status = write_SPI_page_reg_byte_SPI1(page, reg, tmp_val);
    if (status != kStatus_Success)
    {
    	return status;
    }

	return kStatus_Success;
}

int write_SPI_page_reg_bytearray_SPI1(uint8_t page, uint8_t reg, uint8_t* values, uint8_t length)
{
    dspi_transfer_t masterXfer;
    status_t status;

	clear_SPI_buffer_SPI1();
	SPI_masterSendBuffer_SPI1[0]=page;
	SPI_masterSendBuffer_SPI1[1]=reg;
	for (int i=0; i<length; i++)
	{
		SPI_masterSendBuffer_SPI1[i+2]=values[i];
	}

    /*Start master transfer*/
    masterXfer.txData = SPI_masterSendBuffer_SPI1;
    masterXfer.rxData = spi_masterReceiveBuffer_SPI1;
    masterXfer.dataSize = length+2;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

    status = DSPI_RTOS_Transfer(&spi_master_rtos_handle_SPI1, &masterXfer);
    if (status != kStatus_Success)
    {
    	return status;
    }

	return kStatus_Success;
}

int read_SPI_page_reg_bytearray_SPI1(uint8_t page, uint8_t reg, uint8_t* values, uint8_t length)
{
    dspi_transfer_t masterXfer;
    status_t status;

	clear_SPI_buffer_SPI1();
	SPI_masterSendBuffer_SPI1[0]=page | 0x80;
	SPI_masterSendBuffer_SPI1[1]=reg;
	for (int i=0; i<length; i++)
	{
		SPI_masterSendBuffer_SPI1[i+2]=0xFF;
	}

    /*Start master transfer*/
    masterXfer.txData = SPI_masterSendBuffer_SPI1;
    masterXfer.rxData = spi_masterReceiveBuffer_SPI1;
    masterXfer.dataSize = length+2;
    masterXfer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcs0 | kDSPI_MasterPcsContinuous;

    status = DSPI_RTOS_Transfer(&spi_master_rtos_handle_SPI1, &masterXfer);
    if (status != kStatus_Success)
    {
    	return status;
    }

	for (int i=0; i<length; i++)
	{
		values[i]=spi_masterReceiveBuffer_SPI1[i+2];
	}

	return kStatus_Success;
}
