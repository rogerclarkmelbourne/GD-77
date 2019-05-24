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

#include "fw_EEPROM.h"

const uint8_t EEPROM_ADDRESS 	= 0x50;
const uint8_t EEPROM_PAGE_SIZE 	= 128;

// !!! Wait 5ms between consecutive calls !!!
bool EEPROM_Write(int address,uint8_t *buf, int size)
{
	const int COMMAND_SIZE = 2;
	int transferSize;
	uint8_t tmpBuf[COMMAND_SIZE];
    i2c_master_transfer_t masterXfer;
    status_t status;

	taskENTER_CRITICAL();
    while(size > 0)
    {
		transferSize = size>EEPROM_PAGE_SIZE?EEPROM_PAGE_SIZE:size;
		tmpBuf[0] = address >> 8;
		tmpBuf[1] = address & 0xff;

		memset(&masterXfer, 0, sizeof(masterXfer));
		masterXfer.slaveAddress = EEPROM_ADDRESS;
		masterXfer.direction = kI2C_Write;
		masterXfer.subaddress = 0;
		masterXfer.subaddressSize = 0;
		masterXfer.data = tmpBuf;
		masterXfer.dataSize = COMMAND_SIZE;
		masterXfer.flags = kI2C_TransferNoStopFlag;//kI2C_TransferDefaultFlag;

		status = I2C_MasterTransferBlocking(I2C0, &masterXfer);
		if (status != kStatus_Success)
		{
	    	taskEXIT_CRITICAL();
			return false;
		}

		memset(&masterXfer, 0, sizeof(masterXfer));
		masterXfer.slaveAddress = EEPROM_ADDRESS;
		masterXfer.direction = kI2C_Write;
		masterXfer.subaddress = 0;
		masterXfer.subaddressSize = 0;
		masterXfer.data = buf;
		masterXfer.dataSize = transferSize;
		masterXfer.flags = kI2C_TransferNoStartFlag;//kI2C_TransferDefaultFlag;

		status = I2C_MasterTransferBlocking(I2C0, &masterXfer);
		if (status != kStatus_Success)
		{
	    	taskEXIT_CRITICAL();
			return status;
		}
		address += transferSize;
		size -= transferSize;
		if (size!=0)
		{
		    vTaskDelay(portTICK_PERIOD_MS * 5);
		}
    }
	taskEXIT_CRITICAL();
	return true;
}

bool EEPROM_Read(int address,uint8_t *buf, int size)
{
	const int COMMAND_SIZE = 2;
	uint8_t tmpBuf[COMMAND_SIZE];
    i2c_master_transfer_t masterXfer;
    status_t status;

	taskENTER_CRITICAL();
    tmpBuf[0] = address >> 8;
    tmpBuf[1] = address & 0xff;

    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress = EEPROM_ADDRESS;
    masterXfer.direction = kI2C_Write;
    masterXfer.subaddress = 0;
    masterXfer.subaddressSize = 0;
    masterXfer.data = tmpBuf;
    masterXfer.dataSize = COMMAND_SIZE;
    masterXfer.flags = kI2C_TransferNoStopFlag;

    status = I2C_MasterTransferBlocking(I2C0, &masterXfer);
    if (status != kStatus_Success)
    {
    	taskEXIT_CRITICAL();
    	return false;
    }

    memset(&masterXfer, 0, sizeof(masterXfer));
    masterXfer.slaveAddress = EEPROM_ADDRESS;
    masterXfer.direction = kI2C_Read;
    masterXfer.subaddress = 0;
    masterXfer.subaddressSize = 0;
    masterXfer.data = buf;
    masterXfer.dataSize = size;
    masterXfer.flags = kI2C_TransferRepeatedStartFlag;

    status = I2C_MasterTransferBlocking(I2C0, &masterXfer);
    if (status != kStatus_Success)
    {
    	taskEXIT_CRITICAL();
    	return false;
    }

	taskEXIT_CRITICAL();
	return true;
}
