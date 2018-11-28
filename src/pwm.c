/* Author: t.me/dmedser */

#include "pwm.h"
#include "phases.h"

float duty_cycle = 0;

void pwm_init(void) {
	/* fGTM = fPLL = 200 MHz
	 * fPWM = 20 KHz */

	GTM_CMU_CLK_EN.B.EN_FXCLK = 0b10;  /* Enable FXCLK */

	GTM_TOM0_TGC1_ENDIS_CTRL.B.ENDIS_CTRL4 = 0b10;
	GTM_TOM0_TGC1_FUPD_CTRL.B.FUPD_CTRL4   = 0b10;
	GTM_TOM0_TGC1_GLB_CTRL.B.UPEN_CTRL4    = 0b10;
	GTM_TOM0_CH12_SR0.B.SR0                = PWM_PERIOD;
	GTM_TOM0_CH12_CTRL.B.CLK_SRC_SR        = 0b000; /* Channel clock source = FXCLK0 - no division */

	GTM_TOM0_TGC1_ENDIS_CTRL.B.ENDIS_CTRL5 = 0b10;
	GTM_TOM0_TGC1_FUPD_CTRL.B.FUPD_CTRL5   = 0b10;
	GTM_TOM0_TGC1_GLB_CTRL.B.UPEN_CTRL5    = 0b10;
	GTM_TOM0_CH13_SR0.B.SR0                = PWM_PERIOD;
	GTM_TOM0_CH13_CTRL.B.CLK_SRC_SR        = 0b000; /* Channel clock source = FXCLK0 - no division */

	GTM_TOM0_TGC1_ENDIS_CTRL.B.ENDIS_CTRL3 = 0b10;
	GTM_TOM0_TGC1_FUPD_CTRL.B.FUPD_CTRL3   = 0b10;
	GTM_TOM0_TGC1_GLB_CTRL.B.UPEN_CTRL3    = 0b10;
	GTM_TOM0_CH11_SR0.B.SR0                = PWM_PERIOD;
	GTM_TOM0_CH11_CTRL.B.CLK_SRC_SR        = 0b000; /* Channel clock source = FXCLK0 - no division */

	GTM_TOM0_TGC1_GLB_CTRL.B.HOST_TRIG = 0b1; /* Apply the updates */
}

void pwm_off(void) {
	switch_ph(ALL, OFF);
	switch_pl(ALL, OFF);
}

void update_duty_cycle(float new_duty_cycle) {
	duty_cycle = new_duty_cycle;
	GTM_TOM0_CH12_SR1.B.SR1 = (uint16_t)duty_cycle;
	GTM_TOM0_CH13_SR1.B.SR1 = (uint16_t)duty_cycle;
	GTM_TOM0_CH11_SR1.B.SR1 = (uint16_t)duty_cycle;
}
