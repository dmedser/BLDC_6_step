/* Author: t.me/dmedser */

#ifndef PI_CTRL_H_
#define PI_CTRL_H_

#include <stdint.h>

float pi_ctrl(uint32_t target_tpm, uint32_t curr_tpm);
inline void reset_pi_components(void);

#endif /* PI_CTRL_H_ */
