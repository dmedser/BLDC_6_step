#include "timer_angle.h"
#include "isr_priorities.h"
#include <IfxGtm.h>
#include <IfxScuCcu.h>

void timer_angle_init(void) {
	/* Reference angle timer (TBU_CH0) */
	/* PLL angle timer (TIM0_CH0) */

	/* Reference angle timer
	 * Interrupt will be issued every 10 degrees
	 * fREF / fPLL = 1 / 6
	 * CMU_CLK0 divide factor: 600 */
	GTM_CMU_CLK_0_CTRL.B.CLK_CNT = 600 - 1;

	/* Enable CMU_CL0 */
	GTM_CMU_CLK_EN.B.EN_CLK0 = 0b10;

	/* TBU channel 0 clock source: CMU_CLK0 */
	GTM_TBU_CH0_CTRL.B.CH_CLK_SRC = 0b000;

	/* PLL angle timer */
	/* CMU_CLK1 divide factor: 100 */
	GTM_CMU_CLK_1_CTRL.B.CLK_CNT = 100 - 1;
	/* Enable CMU_CLK1 */
	GTM_CMU_CLK_EN.B.EN_CLK1 = 0b10;

	/* TIM0 channel 0 clock source = CMU_CLK1 */
	GTM_TIM0_CH0_CTRL.B.CLK_SEL = 0b001;

	/* Gated Periodic Sampling Mode (TGPS) */
	GTM_TIM0_CH0_CTRL.B.TIM_MODE = 0b101;

	/* Interrupt will be issued every 1 degree if CNT >= CNTS */
	GTM_TIM0_CH0_IRQ_EN.B.NEWVAL_IRQ_EN = 0b1;

	/* Service request priority number (0 - lowest, 0xFF - highest priority) */
	MODULE_SRC.GTM.GTM[0].TIM[0][0].B.SRPN = ISR_PRIORITY_GTM_TIM0_CH0;
	/* Enable service request */
	MODULE_SRC.GTM.GTM[0].TIM[0][0].B.SRE  = 0b1;
	_install_int_handler(ISR_PRIORITY_GTM_TIM0_CH0, (void (*) (int))ISR_timer_angle, 0);

}


/* Rotor angle calculation */
void ISR_timer_angle(void) {

	IfxCpu_forceDisableInterrupts();

	//if(rotor_angle_is_captured) {

		//switch(direction) {
		//	case CLOCKWISE: 	   { rotor_angle_data = /*(rotor_angle_data == 360) ? 0 :*/ rotor_angle_data + 10; break; }
		//	case COUNTERCLOCKWISE: { rotor_angle_data = /*(rotor_angle_data == 0) ? 360 :*/ rotor_angle_data - 10; break; }
		//}

/*		switch(direction_of_rotation) {
			case CLOCKWISE: {
				uint16_t delta = rotor_angle_data + 10;
				switch(current_pattern) {
					case HALL_SENSORS_PATTERN_110: { rotor_angle_data = delta < SECTOR_0_H ? delta : SECTOR_0_H; break; }
					case HALL_SENSORS_PATTERN_100: { rotor_angle_data = delta < SECTOR_1_H ? delta : SECTOR_1_H; break; }
					case HALL_SENSORS_PATTERN_101: { rotor_angle_data = delta < SECTOR_2_H ? delta : SECTOR_2_H; break; }
					case HALL_SENSORS_PATTERN_001: { rotor_angle_data = delta < SECTOR_3_H ? delta : SECTOR_3_H; break; }
					case HALL_SENSORS_PATTERN_011: { rotor_angle_data = delta < SECTOR_4_H ? delta : SECTOR_4_H; break; }
					case HALL_SENSORS_PATTERN_010: { rotor_angle_data = delta < SECTOR_5_H ? delta : SECTOR_5_H; break; }
				}
				break;
			}
			case COUNTERCLOCKWISE: { rotor_angle_data = rotor_angle_data - 10; break; }
		}

	}
*/

	/* Flags must be cleared manually */
	GTM_TIM0_CH0_IRQ_NOTIFY.B.NEWVAL = 0b1;
	GTM_TIM0_CH0_IRQ_NOTIFY.B.GPRzOFL = 0b1;

	IfxCpu_enableInterrupts();
}



void set_leading_vector_angle(uint16_t angle) {
	SECTOR sector;

	/* Specified angle within sector */
	uint16_t sector_angle;

	/* Degrees in sector minus specified angle within it's sector */
	uint8_t alpha;

	if(SECTOR_0_L <= angle && angle <= SECTOR_0_H) {
		sector = SECTOR_0;
		sector_angle = angle - SECTOR_0_L;
		alpha = DEGRESS_IN_SECTOR_0 - sector_angle;
	}
	else if(SECTOR_1_L <= angle && angle <= SECTOR_1_H) {
		sector = SECTOR_1;
		sector_angle = angle - SECTOR_1_L;
		alpha = DEGRESS_IN_SECTOR_1 - sector_angle;
	}
	else if(SECTOR_2_L <= angle && angle <= SECTOR_2_H) {
		sector = SECTOR_2;
		sector_angle = angle - SECTOR_2_L;
		alpha = DEGRESS_IN_SECTOR_2 - sector_angle;
	}
	else if(SECTOR_3_L <= angle && angle <= SECTOR_3_H) {
		sector = SECTOR_3;
		sector_angle = angle - SECTOR_3_L;
		alpha = DEGRESS_IN_SECTOR_3 - sector_angle;
	}
	else if(SECTOR_4_L <= angle && angle <= SECTOR_4_H) {
		sector = SECTOR_4;
		sector_angle = angle - SECTOR_4_L;
		alpha = DEGRESS_IN_SECTOR_4 - sector_angle;
	}
	else if(SECTOR_5_L <= angle && angle <= SECTOR_5_H) {
		sector = SECTOR_5;
		sector_angle = angle - SECTOR_5_L;
		alpha = DEGRESS_IN_SECTOR_5 - sector_angle;
	}
	else if(angle == 360) {
		sector = SECTOR_0;
		sector_angle = 0;
		alpha = DEGRESS_IN_SECTOR_0 - sector_angle;
	}

	float Tb1 = PWM_PERIOD * torque_ampl * sin_table[alpha - 1];
	float Tb2 = PWM_PERIOD * torque_ampl * sin_table[sector_angle];
	float T0  = PWM_PERIOD - Tb1 - Tb2;

	float t1 = Tb1 + Tb2 + T0 / 2.0f;
	float t2 = Tb2 + T0 / 2.0f;
	float t3 = Tb1 + T0 / 2.0f;

	uint16_t Ta = 0;
	uint16_t Tb = 0;
	uint16_t Tc = 0;

	switch(sector) {
		case SECTOR_0: {
			Ta = (uint16_t)t1;
			Tb = (uint16_t)t2;
			Tc = (uint16_t)(T0 / 2.0f);
			break;
		}

		case SECTOR_1: {
			Ta = (uint16_t)t3;
			Tb = (uint16_t)t1;
			Tc = (uint16_t)(T0 / 2.0f);
			break;
		}

		case SECTOR_2: {
			Ta = (uint16_t)(T0 / 2.0f);
			Tb = (uint16_t)t1;
			Tc = (uint16_t)t2;
			break;
		}

		case SECTOR_3: {
			Ta = (uint16_t)(T0 / 2.0f);
			Tb = (uint16_t)t3;
			Tc = (uint16_t)t1;
			break;
		}

		case SECTOR_4: {
			Ta = (uint16_t)t2;
			Tb = (uint16_t)(T0 / 2.0f);
			Tc = (uint16_t)t1;
			break;
		}

		case SECTOR_5: {
			Ta = (uint16_t)t1;
			Tb = (uint16_t)(T0 / 2.0f);
			Tc = (uint16_t)t3;
			break;
		}

		default: {
			Ta = 0;
			Tb = 0;
			Tc = 0;
			break;
		}
	}

	PH_A_H_RE = PWM_PERIOD / 2 - Ta / 2;
	PH_A_H_FE = PWM_PERIOD / 2 + Ta / 2;

	PH_B_H_RE = PWM_PERIOD / 2 - Tb / 2;
	PH_B_H_FE = PWM_PERIOD / 2 + Tb / 2;

	PH_C_H_RE = PWM_PERIOD / 2 - Tc / 2;
	PH_C_H_FE = PWM_PERIOD / 2 + Tc / 2;

	/* Ph_A_H */
	GTM_TOM0_CH11_SR1.B.SR1 = PH_A_H_RE;
	GTM_TOM0_CH11_SR0.B.SR0 = PH_A_H_FE;

	/* Ph_A_L */
	GTM_TOM1_CH4_SR1.B.SR1 = PH_A_H_RE - DEAD_TIME;
	GTM_TOM1_CH4_SR0.B.SR0 = PH_A_H_FE + DEAD_TIME;

	/* Ph_B_H */
	GTM_TOM0_CH13_SR1.B.SR1 = PH_B_H_RE;
	GTM_TOM0_CH13_SR0.B.SR0 = PH_B_H_FE;

	/* Ph_B_L */
	GTM_TOM1_CH3_SR1.B.SR1 = PH_B_H_RE - DEAD_TIME;
	GTM_TOM1_CH3_SR0.B.SR0 = PH_B_H_FE + DEAD_TIME;

	/* Ph_C_H */
	GTM_TOM0_CH12_SR1.B.SR1 = PH_C_H_RE;
	GTM_TOM0_CH12_SR0.B.SR0 = PH_C_H_FE;

	/* Ph_C_L */
	GTM_TOM1_CH2_SR1.B.SR1 = PH_C_H_RE - DEAD_TIME;
	GTM_TOM1_CH2_SR0.B.SR0 = PH_C_H_FE + DEAD_TIME;

}


void update_leading_vector_angle(void) {

	int32_t leading_vector_angle = (int32_t)rotor_angle + torque_delta;

	leading_vector_angle = (leading_vector_angle > 360) ? (leading_vector_angle - 360) :
						   ((leading_vector_angle < 0)  ? (360 + leading_vector_angle) : leading_vector_angle);

	set_leading_vector_angle((uint16_t)leading_vector_angle);

}


void launch_rotor(void) {
	set_leading_vector_angle(get_launch_angle());
}


float sin_table[MAX_NUMBER_OF_INTEGER_ANGLES_IN_THE_SECTOR] = {
		0.0000f, // 0
		0.0175f, // 1
		0.0349f, // 2
		0.0523f, // 3
		0.0698f, // 4
		0.0872f, // 5
		0.1045f, // 6
		0.1219f, // 7
		0.1392f, // 8
		0.1564f, // 9
		0.1736f, // 10
		0.1908f, // 11
		0.2079f, // 12
		0.2250f, // 13
		0.2419f, // 14
		0.2588f, // 15
		0.2756f, // 16
		0.2924f, // 17
		0.3090f, // 18
		0.3256f, // 19
		0.3420f, // 20
		0.3584f, // 21
		0.3746f, // 22
		0.3907f, // 23
		0.4067f, // 24
		0.4226f, // 25
		0.4384f, // 26
		0.4540f, // 27
		0.4695f, // 28
		0.4848f, // 29
		0.5000f, // 30
		0.5150f, // 31
		0.5299f, // 32
		0.5446f, // 33
		0.5592f, // 34
		0.5736f, // 35
		0.5878f, // 36
		0.6018f, // 37
		0.6157f, // 38
		0.6293f, // 39
		0.6428f, // 40
		0.6561f, // 41
		0.6691f, // 42
		0.6820f, // 43
		0.6947f, // 44
		0.7071f, // 45
		0.7193f, // 46
		0.7314f, // 47
		0.7431f, // 48
		0.7547f, // 49
		0.7660f, // 50
		0.7771f, // 51
		0.7880f, // 52
		0.7986f, // 53
		0.8090f, // 54
		0.8192f, // 55
		0.8290f, // 56
		0.8387f, // 57
		0.8480f, // 58
		0.8572f, // 59
		0.8660f, // 60
		0.8746f, // 61
		0.8829f, // 62
		0.8910f, // 63
		0.8988f, // 64
		0.9063f, // 65
		0.9135f, // 66
		0.9205f, // 67
		0.9272f, // 68
		0.9336f, // 69
		0.9397f  // 70
};