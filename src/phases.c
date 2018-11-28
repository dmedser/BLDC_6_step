/* Author: t.me/dmedser */

#include "phases.h"
#include "pwm.h"
#include "ports.h"
#include "hall_sensors.h"
#include "stm.h"

void switch_ph(phase ph, switch_action state) {
	switch(ph) {
	case AH:
		PH_A_H_OUT_EN(state);
		break;
	case BH:
		PH_B_H_OUT_EN(state);
		break;
	case CH:
		PH_C_H_OUT_EN(state);
		break;
	case ALL:
		PH_A_H_OUT_EN(state);
		PH_B_H_OUT_EN(state);
		PH_C_H_OUT_EN(state);
		break;
	default:
		break;
	}
	GTM_TOM0_TGC1_GLB_CTRL.B.HOST_TRIG = 0b1;
}


void switch_pl(phase pl, switch_action state) {
	switch(pl) {
	case AL:
		SET_PIN_STATE(PH_A_L, state);
		break;
	case BL:
		SET_PIN_STATE(PH_B_L, state);
		break;
	case CL:
		SET_PIN_STATE(PH_C_L, state);
		break;
	case ALL:
		SET_PIN_STATE(PH_A_L, state);
		SET_PIN_STATE(PH_B_L, state);
		SET_PIN_STATE(PH_C_L, state);
		break;
	default:
		break;
	}
}


void switch_phases(phase ph, phase pl) {

	switch_ph(ALL, OFF);
	switch_pl(ALL, OFF);

	/* fSTM = (fSRC / STMDIV)
	 * STMDIV by default = 2
	 * fSRC   by default = fPLL = IFX_CFG_SCU_PLL_FREQUENCY = 200 MHz
	 * fSTM = 200 MHz / 2 = 100 MHz
	 */
	#define STM_TICKS_IN_1_US  100

	stm.delay(STM_TICKS_IN_1_US);

	switch(pl) {
	case AL:
		switch_pl(AL, ON);
		switch_pl(CL, OFF);
		break;
	case BL:
		switch_pl(BL, ON);
		switch_pl(AL, OFF);
		break;
	case CL:
		switch_pl(CL, ON);
		switch_pl(BL, OFF);
		break;
	default:
		break;
	}

	switch(ph) {
	case AH:
		switch_ph(AH, ON);
		switch_ph(CH, OFF);
		break;
	case BH:
		switch_ph(BH, ON);
		switch_ph(AH, OFF);
		break;
	case CH:
		switch_ph(CH, ON);
		switch_ph(BH, OFF);
		break;
	default:
		break;
	}
}


void change_leading_vector(void) {
	switch(current_pattern) {
		case HALL_SENSORS_PATTERN_110:
			switch_phases(BH, AL);
			break;
		case HALL_SENSORS_PATTERN_100:
			switch_phases(CH, AL);
			break;
		case HALL_SENSORS_PATTERN_101:
			switch_phases(CH, BL);
			break;
		case HALL_SENSORS_PATTERN_001:
			switch_phases(AH, BL);
			break;
		case HALL_SENSORS_PATTERN_011:
			switch_phases(AH, CL);
			break;
		case HALL_SENSORS_PATTERN_010:
			switch_phases(BH, CL);
			break;
	}
}

