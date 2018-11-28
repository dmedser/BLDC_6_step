/* Author: t.me/dmedser */

#ifndef HALL_SENSORS_H_
#define HALL_SENSORS_H_

/* Hall sensors [2:0] */
typedef enum {
	HALL_SENSORS_PATTERN_001 = 0b001,
	HALL_SENSORS_PATTERN_011 = 0b011,
	HALL_SENSORS_PATTERN_010 = 0b010,
	HALL_SENSORS_PATTERN_110 = 0b110,
	HALL_SENSORS_PATTERN_100 = 0b100,
	HALL_SENSORS_PATTERN_101 = 0b101
} HALL_SENSORS_PATTERN;

void hall_sensors_init(void);

void update_hall_sensors_patterns(void);

extern HALL_SENSORS_PATTERN current_pattern;

#endif /* HALL_SENSORS_H_ */

