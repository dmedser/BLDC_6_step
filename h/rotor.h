/* Author: t.me/dmedser */

#ifndef ROTOR_H_
#define ROTOR_H_

#include <stdint.h>
#include <Platform_Types.h>

typedef struct rotor_t {
	uint16_t target_turns_per_min;
	uint16_t current_turns_per_min;
	float calibration_multiplier;
	float calibration_offset;
	boolean is_launched;
	boolean (*is_stopped)(void);
	void (*update_current_turns_per_min)(void);
	void (*sample_turns_per_min)(void);
	void (*launch)(void);
} rotor_t;

extern rotor_t rotor;

#endif /* ROTOR_H_ */
