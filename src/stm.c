/* Author: t.me/dmedser */

#include "stm.h"
#include <IfxStm.h>

static Ifx_STM *module_stm_0 = &MODULE_STM0;

static void stm_init(void) {
	stm.frequency = (uint32_t)IfxStm_getFrequency(module_stm_0);
}

static uint32_t stm_get_value(void) {
	return (uint32_t)IfxStm_getLower(module_stm_0);
}

static void stm_delay(uint32_t ticks) {
	IfxStm_waitTicks(module_stm_0, ticks);
}

stm_t stm = {stm_init, 0, 0, 0, stm_get_value, stm_delay};
