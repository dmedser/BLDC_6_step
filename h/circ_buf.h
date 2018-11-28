/* Author: t.me/dmedser */

#ifndef CIRC_BUF_H_
#define CIRC_BUF_H_

#include <stdint.h>

#define CIRC_BUF_SIZE	80

typedef struct circ_buf_t {
	uint16_t buf[CIRC_BUF_SIZE];
	uint8_t idx;
	void (*add)(uint16_t val);
	uint16_t (*get_avg)(void);
	void (*clear)(void);
} circ_buf_t;

extern circ_buf_t circ_buf;

#endif /* CIRC_BUF_H_ */
