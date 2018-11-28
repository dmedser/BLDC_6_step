/* Author: t.me/dmedser */

#ifndef STM_H_
#define STM_H_

#include <stdint.h>

typedef struct stm_t {
	void (*init)(void);
	uint32_t frequency;
	uint32_t ticks_per_sector;
	uint32_t last_pattern_switch_timestamp;
	uint32_t (*get_value)(void);
	void (*delay)(uint32_t ticks);
} stm_t;

extern stm_t stm;

#endif /* STM_H_ */
