/* Author: t.me/dmedser */

#include "scu_clk_cfg.h"
#include "stm.h"
#include "gtm.h"
#include "pwm.h"
#include "hall_sensors.h"
#include "ports.h"
#include "rtos.h"
#include "can.h"
#include "rotor.h"
#include "dflash.h"
#include <IfxCpu.h>
#include <machine/cint.h>

int main(void) {
	IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

	IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

	IfxCpu_disableInterrupts();

	_init_vectab();

	scu_clk_init();

	stm.init();

	gtm_init();

	pwm_init();

	hall_sensors_init();

	ports_init();

	rtos_init();

	can_init();

	download_from_dflash(CALIBRATION_MULTIPLIER);

	download_from_dflash(CALIBRATION_OFFSET);

	IfxCpu_enableInterrupts();

	while(1);
}
