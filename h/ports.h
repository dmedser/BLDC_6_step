/* Author: t.me/dmedser */

#ifndef PORTS_H_
#define PORTS_H_

#include <IfxPort.h>

#define SET_PIN_STATE(PIN, STATE)  IfxPort_setPinState(PIN, (STATE == ON) ?\
                                   IfxPort_State_high : IfxPort_State_low)

#define GET_HS_STATE(HS)           IfxPort_getPinState(HS)

#define PH_A_L  &MODULE_P11, 3
#define PH_B_L  &MODULE_P11, 6
#define PH_C_L  &MODULE_P11, 9

#define HS_0    &MODULE_P02, 6
#define HS_1    &MODULE_P02, 7
#define HS_2    &MODULE_P02, 8

void ports_init(void);

#endif /* PORTS_H_ */
