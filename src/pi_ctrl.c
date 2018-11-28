/* Author: t.me/dmedser */

#include "pi_ctrl.h"

static float P = 0.0f;
static float I = 0.0f;

static float Ki = 0.00007f;
static float Kp = 0.008f;

#define I_MAX  2
#define I_MIN  -I_MAX

float pi_ctrl(uint32_t target_tpm, uint32_t curr_tpm) {

	int32_t err = (int32_t)target_tpm - (int32_t)curr_tpm;

	P = err * Kp;

	I += err * Ki;

	if(I > I_MAX) {
		I = I_MAX;
	}
	else if(I < I_MIN) {
		I = I_MIN;
	}

	return P + I;
}


inline void reset_pi_components(void) {
	P = 0.0f;
	I = 0.0f;
}
