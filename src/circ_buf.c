/* Author: t.me/dmedser */

#include "circ_buf.h"

static void circ_buf_add(uint16_t val) {
	circ_buf.buf[circ_buf.idx] = val;
	circ_buf.idx = (circ_buf.idx + 1) % CIRC_BUF_SIZE;
}

static uint16_t circ_buf_get_avg(void) {
	uint32_t sum = 0;
	uint8_t i = 0;
	for(; i < CIRC_BUF_SIZE; i++) {
		sum += circ_buf.buf[i];
	}
	return (uint16_t)(sum / CIRC_BUF_SIZE);
}

static void circ_buf_clear(void) {
	uint8_t i = 0;
	for(; i < CIRC_BUF_SIZE; i++) {
		circ_buf.buf[i] = 0;
	}
	circ_buf.idx = 0;
}

circ_buf_t circ_buf = {{0}, 0, circ_buf_add, circ_buf_get_avg, circ_buf_clear};
