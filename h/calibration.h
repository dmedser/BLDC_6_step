/* Author: t.me/dmedser */

#ifndef CALIBRATION_H_
#define CALIBRATION_H_

#include <stdint.h>

#define LEVEL_1_MS_FACTOR       1000
#define LEVEL_10_MS_FACTOR      100
#define LEVEL_100_MS_FACTOR     10
#define LEVEL_FACTOR            LEVEL_100_MS_FACTOR

#define CAL_STEP_DURATION_SEC   20
#define NEXT_STEP_TIME          ((rtos_time_cntr - last_timestamp) == (CAL_STEP_DURATION_SEC * LEVEL_FACTOR))

#define CAL_DC_STEP_1           250
#define CAL_DC_STEP_2           300
#define CAL_DC_STEP_3           350

typedef enum calibration_state_t {
	IDLE,
	START,
	STEP_1,
	STEP_2,
	STEP_3,
	COMPLETION
} calibration_state_t;

void calibration_process(uint32_t rtos_time_cntr);

extern calibration_state_t calibration_state;

#endif /* CALIBRATION_H_ */
