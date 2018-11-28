/* Author: t.me/dmedser */

#include "calibration.h"
#include "dflash.h"
#include "rotor.h"
#include "pwm.h"

static uint32_t last_timestamp = 0;
static uint16_t tpm_steps[3] = {0};
static float multiplier_steps[3] = {0};
static float offset_steps[3] = {0};

calibration_state_t calibration_state = IDLE;

void calibration_process(uint32_t rtos_time_cntr) {
	switch(calibration_state) {
	case IDLE:
		break;
	case START:
		last_timestamp = rtos_time_cntr;
		update_duty_cycle(CAL_DC_STEP_1);
		calibration_state = STEP_1;
		break;
	case STEP_1:
		if(NEXT_STEP_TIME) {
			last_timestamp = rtos_time_cntr;
			tpm_steps[0] = rotor.current_turns_per_min;
			update_duty_cycle(CAL_DC_STEP_2);
			calibration_state = STEP_2;
		}
		break;
	case STEP_2:
		if(NEXT_STEP_TIME) {
			last_timestamp = rtos_time_cntr;
			tpm_steps[1] = rotor.current_turns_per_min;
			update_duty_cycle(CAL_DC_STEP_3);
			calibration_state = STEP_3;
		}
		break;
	case STEP_3:
		if(NEXT_STEP_TIME) {
			tpm_steps[2] = rotor.current_turns_per_min;;
			update_duty_cycle(0);
			calibration_state = COMPLETION;
		}
		break;
	case COMPLETION:
		multiplier_steps[0] = (float)(tpm_steps[1] - tpm_steps[0]) / (float)(CAL_DC_STEP_2 - CAL_DC_STEP_1);
		multiplier_steps[1] = (float)(tpm_steps[2] - tpm_steps[1]) / (float)(CAL_DC_STEP_3 - CAL_DC_STEP_2);
		multiplier_steps[2] = (float)(tpm_steps[2] - tpm_steps[0]) / (float)(CAL_DC_STEP_3 - CAL_DC_STEP_1);

		float multiplier = (multiplier_steps[0] + multiplier_steps[1] + multiplier_steps[2]) / 3.0f;

		offset_steps[0] = tpm_steps[0] - multiplier * CAL_DC_STEP_1;
		offset_steps[1] = tpm_steps[1] - multiplier * CAL_DC_STEP_2;
		offset_steps[2] = tpm_steps[2] - multiplier * CAL_DC_STEP_3;

		float offset = (offset_steps[0] + offset_steps[1] + offset_steps[2]) / 3.0f;

		rotor.calibration_multiplier = multiplier;
		rotor.calibration_offset = offset;

		upload_into_dflash(CALIBRATION_MULTIPLIER);
		upload_into_dflash(CALIBRATION_OFFSET);

		calibration_state = IDLE;

		break;
	default:
		break;
	}
}

