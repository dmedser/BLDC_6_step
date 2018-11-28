/* Author: t.me/dmedser */

#include "hall_sensors.h"
#include "isr_priorities.h"
#include "ports.h"
#include "phases.h"
#include "rotor.h"
#include <stdint.h>
#include <IfxCcu6.h>
#include <IfxSrc_reg.h>
#include <machine/cint.h>

#define HALL_SENSOR_0_STATE  GET_HS_STATE(HS_0)
#define HALL_SENSOR_1_STATE  GET_HS_STATE(HS_1)
#define HALL_SENSOR_2_STATE  GET_HS_STATE(HS_2)

HALL_SENSORS_PATTERN current_pattern;

static void ISR_correct_hall_event(void);

void hall_sensors_init(void) {
	IfxCcu6_enableModule(&MODULE_CCU60);

	IfxCcu6_enableInterrupt(&MODULE_CCU60, IfxCcu6_InterruptSource_correctHallEvent);
	IfxCcu6_routeInterruptNode(&MODULE_CCU60, IfxCcu6_InterruptSource_correctHallEvent, IfxCcu6_ServiceRequest_3);

	/* Hall Sensor Mode, in  order to properly enable this mode, all
	 * three MSEL6x fields have to be programmed to Hall Sensor mode */

	IfxCcu6_setT12ChannelMode(&MODULE_CCU60, IfxCcu6_T12Channel_0, IfxCcu6_T12ChannelMode_hallSensor);
	IfxCcu6_setT12ChannelMode(&MODULE_CCU60, IfxCcu6_T12Channel_1, IfxCcu6_T12ChannelMode_hallSensor);
	IfxCcu6_setT12ChannelMode(&MODULE_CCU60, IfxCcu6_T12Channel_2, IfxCcu6_T12ChannelMode_hallSensor);

	IfxCcu6_enableDeadTime(&MODULE_CCU60, IfxCcu6_T12Channel_0);
	IfxCcu6_enableDeadTime(&MODULE_CCU60, IfxCcu6_T12Channel_1);
	IfxCcu6_enableDeadTime(&MODULE_CCU60, IfxCcu6_T12Channel_2);

	IfxCcu6_setDeadTimeValue(&MODULE_CCU60, 10);

	IfxCcu6_enableDelayBypass(&MODULE_CCU60);

	MODULE_SRC.CCU6.CCU6[0].SR3.B.SRPN = ISR_PRIORITY_CORRECT_HALL_EVENT;
	MODULE_SRC.CCU6.CCU6[0].SR3.B.SRE  = 0b1;
	_install_int_handler(ISR_PRIORITY_CORRECT_HALL_EVENT, (void (*) (int))ISR_correct_hall_event, 0);

	update_hall_sensors_patterns();
}


static HALL_SENSORS_PATTERN get_current_hall_sensors_pattern(void) {
	return (HALL_SENSORS_PATTERN)((HALL_SENSOR_2_STATE << 2) |
	                              (HALL_SENSOR_1_STATE << 1) |
	                               HALL_SENSOR_0_STATE);
}


static HALL_SENSORS_PATTERN get_expected_hall_sensors_pattern(void) {
	switch(current_pattern) {
	case HALL_SENSORS_PATTERN_001: return HALL_SENSORS_PATTERN_011;
	case HALL_SENSORS_PATTERN_011: return HALL_SENSORS_PATTERN_010;
	case HALL_SENSORS_PATTERN_010: return HALL_SENSORS_PATTERN_110;
	case HALL_SENSORS_PATTERN_110: return HALL_SENSORS_PATTERN_100;
	case HALL_SENSORS_PATTERN_100: return HALL_SENSORS_PATTERN_101;
	case HALL_SENSORS_PATTERN_101: return HALL_SENSORS_PATTERN_001;
	}
}


void update_hall_sensors_patterns(void) {
	current_pattern = get_current_hall_sensors_pattern();
	IfxCcu6_setCurrentHallPattern(&MODULE_CCU60, (uint8_t)current_pattern);

	HALL_SENSORS_PATTERN expected_pattern = get_expected_hall_sensors_pattern();
	IfxCcu6_setExpectedHallPattern(&MODULE_CCU60, (uint8_t)expected_pattern);

	IfxCcu6_enableHallPatternShadowTransfer(&MODULE_CCU60);
}


static void ISR_correct_hall_event(void) {
	IfxCpu_forceDisableInterrupts();

	rotor.sample_turns_per_min();

	rotor.launch();

	if(rotor.is_launched == FALSE) {
		rotor.is_launched = TRUE;
	}

	IfxCpu_enableInterrupts();
}

