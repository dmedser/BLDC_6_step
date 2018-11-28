/* Author: t.me/dmedser */

#ifndef DFLASH_H_
#define DFLASH_H_

#include <stdint.h>

typedef uint8_t parameter_idx;

#define CALIBRATION_MULTIPLIER   0
#define CALIBRATION_OFFSET       1

void upload_into_dflash(parameter_idx parameter);
void download_from_dflash(parameter_idx parameter);

#endif /* DFLASH_H_ */
