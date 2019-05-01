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

#include "fw_i2c.h"

#define I2C_BAUDRATE (100000) /* 100K */
#define I2C_MASTER_SLAVE_ADDR_7BIT (0x71U)

#define I2C_DATA_LENGTH (32)  /* MAX is 256 */
uint8_t i2c_master_buff[I2C_DATA_LENGTH];

i2c_rtos_handle_t i2c_master_rtos_handle;

void init_I2C0a()
{
    // I2C0a to AT24C512 EEPROM & AT1846S
    const port_pin_config_t porte24_config = {/* Internal pull-up resistor is enabled */
                                              kPORT_PullUp,
                                              /* Fast slew rate is configured */
                                              kPORT_FastSlewRate,
                                              /* Passive filter is disabled */
                                              kPORT_PassiveFilterDisable,
                                              /* Open drain is enabled */
                                              kPORT_OpenDrainEnable,
                                              /* Low drive strength is configured */
                                              kPORT_LowDriveStrength,
                                              /* Pin is configured as I2C0_SCL */
                                              kPORT_MuxAlt5,
                                              /* Pin Control Register fields [15:0] are not locked */
                                              kPORT_UnlockRegister};
    PORT_SetPinConfig(Port_I2C0a_SCL, Pin_I2C0a_SCL, &porte24_config);

    const port_pin_config_t porte25_config = {/* Internal pull-up resistor is enabled */
                                              kPORT_PullUp,
                                              /* Fast slew rate is configured */
                                              kPORT_FastSlewRate,
                                              /* Passive filter is disabled */
                                              kPORT_PassiveFilterDisable,
                                              /* Open drain is enabled */
                                              kPORT_OpenDrainEnable,
                                              /* Low drive strength is configured */
                                              kPORT_LowDriveStrength,
                                              /* Pin is configured as I2C0_SDA */
                                              kPORT_MuxAlt5,
                                              /* Pin Control Register fields [15:0] are not locked */
                                              kPORT_UnlockRegister};
    PORT_SetPinConfig(Port_I2C0a_SDA, Pin_I2C0a_SDA, &porte25_config);

    NVIC_SetPriority(I2C0_IRQn, 3);
}

void init_I2C0b()
{
	// I2C0b to ALPU-MP-1413
    const port_pin_config_t portb2_config = {/* Internal pull-up resistor is enabled */
                                             kPORT_PullUp,
                                             /* Fast slew rate is configured */
                                             kPORT_FastSlewRate,
                                             /* Passive filter is disabled */
                                             kPORT_PassiveFilterDisable,
                                             /* Open drain is enabled */
                                             kPORT_OpenDrainEnable,
                                             /* Low drive strength is configured */
                                             kPORT_LowDriveStrength,
                                             /* Pin is configured as I2C0_SCL */
                                             kPORT_MuxAlt2,
                                             /* Pin Control Register fields [15:0] are not locked */
                                             kPORT_UnlockRegister};
    PORT_SetPinConfig(Port_I2C0b_SCL, Pin_I2C0b_SCL, &portb2_config);

    const port_pin_config_t portb3_config = {/* Internal pull-up resistor is enabled */
                                             kPORT_PullUp,
                                             /* Fast slew rate is configured */
                                             kPORT_FastSlewRate,
                                             /* Passive filter is disabled */
                                             kPORT_PassiveFilterDisable,
                                             /* Open drain is enabled */
                                             kPORT_OpenDrainEnable,
                                             /* Low drive strength is configured */
                                             kPORT_LowDriveStrength,
                                             /* Pin is configured as I2C0_SDA */
                                             kPORT_MuxAlt2,
                                             /* Pin Control Register fields [15:0] are not locked */
                                             kPORT_UnlockRegister};
    PORT_SetPinConfig(Port_I2C0b_SDA, Pin_I2C0b_SDA, &portb3_config);

    NVIC_SetPriority(I2C0_IRQn, 3);
}

int setup_I2C0()
{
    i2c_master_config_t masterConfig;
    status_t status;

	/*
	 * masterConfig.baudRate_Bps = 100000U;
	 * masterConfig.enableStopHold = false;
	 * masterConfig.glitchFilterWidth = 0U;
	 * masterConfig.enableMaster = true;
	 */
	I2C_MasterGetDefaultConfig(&masterConfig);
	masterConfig.baudRate_Bps = I2C_BAUDRATE;

	status = I2C_RTOS_Init(&i2c_master_rtos_handle, I2C0, &masterConfig, CLOCK_GetFreq(I2C0_CLK_SRC));
	if (status != kStatus_Success)
	{
    	return status;
    }

	return kStatus_Success;
}
