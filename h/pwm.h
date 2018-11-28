/* Author: t.me/dmedser */

#ifndef TIMER_PWM_H_
#define TIMER_PWM_H_

#include <stdint.h>
#include <IfxGtm.h>

#define PWM_PERIOD            10000 /* Period in ticks of 200 MHz timer, fPWM = 20 KHz */

#define PH_A_H_OUT_EN(STATE)  GTM_TOM0_TGC1_OUTEN_CTRL.B.OUTEN_CTRL4 = (STATE == ON) ?\
                              0b10 : 0b01
#define PH_B_H_OUT_EN(STATE)  GTM_TOM0_TGC1_OUTEN_CTRL.B.OUTEN_CTRL5 = (STATE == ON) ?\
                              0b10 : 0b01
#define PH_C_H_OUT_EN(STATE)  GTM_TOM0_TGC1_OUTEN_CTRL.B.OUTEN_CTRL3 = (STATE == ON) ?\
                              0b10 : 0b01

void pwm_init(void);
void pwm_off(void);

void update_duty_cycle(float new_duty_cycle);
extern float duty_cycle;

#endif /* TIMER_PWM_H_ */
