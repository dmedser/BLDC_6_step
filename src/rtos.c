/* Author: t.me/dmedser */

#include "rtos.h"
#include "isr_priorities.h"
#include "rotor.h"
#include "pwm.h"
#include "circ_buf.h"
#include "pi_ctrl.h"
#include "calibration.h"
#include <IfxCpu.h>
#include <IfxGpt12_reg.h>
#include <IfxSrc_reg.h>
#include <machine/cint.h>

static void ISR_rtos_100_us(void);
static void ISR_rtos_1_ms(void);
static void ISR_rtos_10_ms(void);
static void ISR_rtos_100_ms(void);

static uint32_t cnt_100_us = 0;
static uint32_t cnt_1_ms = 0;
static uint32_t cnt_100_ms = 0;

/* RTOS 100 us level */
static void rtos_base_init(void) {
	IfxCpu_disableInterrupts();

	uint16_t password = IfxScuWdt_getCpuWatchdogPassword();

	IfxScuWdt_clearCpuEndinit(password);
	MODULE_GPT120.CLC.B.DISR = 0b0;       /* Enable control of the module */

	/* GPT2 */
	/* fGPT = fSPB = fPLL / 2 (default) = 200 MHz / 2 = 100 MHz */
	/* Core timer T6 of GPT2 configuration */
	MODULE_GPT120.T6CON.B.BPS2  = 0b01;   /* Prescaler: fGPT2 = fGPT/2 = 50 MHz */
	MODULE_GPT120.T6CON.B.T6M   = 0b000;  /* Timer mode */
	MODULE_GPT120.T6CON.B.T6I   = 0b000;  /* Input: fGPT2 = fGPT/2 */
	MODULE_GPT120.T6CON.B.T6UD  = 0b0;    /* T6 counts up */
	MODULE_GPT120.T6CON.B.T6UDE = 0b0;    /* T6 count direction is controlled by T6UD */
	MODULE_GPT120.T6CON.B.T6OE  = 0b0;    /* T6 overflow output disable */
	MODULE_GPT120.T6CON.B.T6OTL = 0b0;    /* Overflow toggle latch initial state 0 */
	MODULE_GPT120.T6CON.B.T6CLR = 0b0;    /* T6 is not cleared on a capture event */
	MODULE_GPT120.T6CON.B.T6SR  = 0b1;    /* Enable reload from CAPREL */
	MODULE_GPT120.T6.U = 65535 - 5000;    /* 100 us */

	MODULE_GPT120.CAPREL.U = 65535 - 5000;

	IfxScuWdt_setCpuEndinit(password);

	MODULE_SRC.GPT12.GPT12[0].T6.B.SRPN = ISR_PN_RTOS_100_US;
	MODULE_SRC.GPT12.GPT12[0].T6.B.SRE  = 0b1;
	_install_int_handler(ISR_PN_RTOS_100_US, (void (*) (int))ISR_rtos_100_us, 0);

	MODULE_GPT120.T6CON.B.T6R = 0b1;      /* Run timer */
}


static void rtos_service_requests_init(void) {
	/* RTOS 1 ms level - SR 0 */
	MODULE_SRC.GPSR.GPSR[0].SR0.B.SRPN = ISR_PN_RTOS_1_MS;
	MODULE_SRC.GPSR.GPSR[0].SR0.B.SRE  = 0b1;
	_install_int_handler(ISR_PN_RTOS_1_MS, (void (*) (int))ISR_rtos_1_ms, 0);

	/* RTOS 10 ms - SR 1 */
	MODULE_SRC.GPSR.GPSR[0].SR1.B.SRPN = ISR_PN_RTOS_10_MS;
	MODULE_SRC.GPSR.GPSR[0].SR1.B.SRE  = 0b1;
	_install_int_handler(ISR_PN_RTOS_10_MS, (void (*) (int))ISR_rtos_10_ms, 0);

	/* RTOS 100 ms - SR 2 */
	MODULE_SRC.GPSR.GPSR[0].SR2.B.SRPN = ISR_PN_RTOS_100_MS;
	MODULE_SRC.GPSR.GPSR[0].SR2.B.SRE  = 0b1;
	_install_int_handler(ISR_PN_RTOS_100_MS, (void (*) (int))ISR_rtos_100_ms, 0);
}


void rtos_init(void) {
	rtos_base_init();
	rtos_service_requests_init();
}


static void ISR_rtos_100_us(void){
	IfxCpu_forceDisableInterrupts();

	cnt_100_us++;

	/* Call 1 ms level - service request 0 */
	if(cnt_100_us == 10) {
		cnt_100_us = 0;
		MODULE_SRC.GPSR.GPSR[0].SR0.B.SETR = 0b1;
	}

	IfxCpu_enableInterrupts();
}


static void ISR_rtos_1_ms(void) {
	IfxCpu_forceDisableInterrupts();

	cnt_1_ms++;

	/* Call RTOS 10 ms level - service request 1 */
	if(cnt_1_ms % 10 == 0) {
		MODULE_SRC.GPSR.GPSR[0].SR1.B.SETR = 0b1;
	}

	/* Call RTOS 100 ms level - service request 2 */
	if(cnt_1_ms == 100) {
		cnt_1_ms = 0;
		MODULE_SRC.GPSR.GPSR[0].SR2.B.SETR = 0b1;
	}

	IfxCpu_enableInterrupts();
}


static void ISR_rtos_10_ms(void) {
	IfxCpu_forceDisableInterrupts();

	if(rotor.is_launched) {
		if(rotor.is_stopped()) {
			update_duty_cycle(0);
			pwm_off();
			reset_pi_components();
			rotor.is_launched = FALSE;
			circ_buf.clear();
			rotor.current_turns_per_min = 0;
		}
		else {
			rotor.update_current_turns_per_min();
		}
	}
	else {
		if((rotor.target_turns_per_min > 0) || (calibration_state != IDLE)) {
			rotor.launch();
		}
	}

	IfxCpu_enableInterrupts();
}


static inline void pi_ctrl_process(void) {
	if(rotor.target_turns_per_min > 0) {
		float new_duty_cycle = duty_cycle + pi_ctrl(rotor.target_turns_per_min, rotor.current_turns_per_min);
		update_duty_cycle((new_duty_cycle >= 0) ? new_duty_cycle : 0);
	}
}


static void ISR_rtos_100_ms(void) {
	IfxCpu_forceDisableInterrupts();

	cnt_100_ms++;

	if(calibration_state == IDLE) {
		pi_ctrl_process();
	}
	else {
		calibration_process(cnt_100_ms);
	}

	IfxCpu_enableInterrupts();
}

