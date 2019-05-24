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

#include "fw_wdog.h"

static WDOG_Type *wdog_base = WDOG;
int watchdog_refresh_tick=0;

int battery_voltage = 0;
int battery_voltage_tick = 0;

void init_watchdog()
{
    wdog_config_t config;
    WDOG_GetDefaultConfig(&config);
    config.timeoutValue = 0x3ffU;
    WDOG_Init(wdog_base, &config);
    for (uint32_t i = 0; i < 256; i++)
    {
    	wdog_base->RSTCNT;
    }

    watchdog_refresh_tick=0;

	battery_voltage=get_battery_voltage();
	battery_voltage_tick=0;
}

void tick_watchdog()
{
	watchdog_refresh_tick++;
	if (watchdog_refresh_tick==200)
	{
    	WDOG_Refresh(wdog_base);
    	watchdog_refresh_tick=0;
	}

	battery_voltage_tick++;
	if (battery_voltage_tick==2000)
	{
		int tmp_battery_voltage = get_battery_voltage();
		if (battery_voltage!=tmp_battery_voltage)
		{
			battery_voltage=tmp_battery_voltage;
			if ((Shutdown_Timer==0) && (SplashScreen_Timer==0))
			{
				update_flags();
			}
		}
		battery_voltage_tick=0;
	}
}
