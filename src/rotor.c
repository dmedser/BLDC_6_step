/* Author: t.me/dmedser */

#include "rotor.h"
#include "stm.h"
#include "circ_buf.h"
#include "phases.h"
#include "hall_sensors.h"

static boolean rotor_is_stopped(void) {
	return (stm.get_value() - stm.last_pattern_switch_timestamp) > (3 * stm.ticks_per_sector);
}

static void rotor_update_current_turns_per_min(void) {
	rotor.current_turns_per_min = circ_buf.get_avg();
}

static void rotor_sample_turns_per_min(void) {
	stm.ticks_per_sector = stm.get_value() - stm.last_pattern_switch_timestamp;
	stm.last_pattern_switch_timestamp = stm.get_value();
	uint16_t turns_per_min_raw = (uint16_t)(10.0f / ((float)stm.ticks_per_sector / stm.frequency));
	circ_buf.add(turns_per_min_raw);
}

static void rotor_launch(void) {
	update_hall_sensors_patterns();
	change_leading_vector();
}

rotor_t rotor = {0, 0, 0.0f, 0.0f, FALSE,
	rotor_is_stopped,
	rotor_update_current_turns_per_min,
	rotor_sample_turns_per_min,
	rotor_launch};
