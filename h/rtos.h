/* Author: t.me/dmedser */

#ifndef RTOS_H_
#define RTOS_H_

void rtos_init(void);

#include <stdint.h>

extern uint32_t last_can_cmd_timestamp;
extern uint32_t cnt_100_ms;

#endif /* RTOS_H_ */
