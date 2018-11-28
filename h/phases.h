/* Author: t.me/dmedser */

#ifndef PHASES_H_
#define PHASES_H_

#include <Platform_Types.h>

typedef boolean switch_action;
#define ON  TRUE
#define OFF FALSE

typedef enum phase {
	AH, AL,
	BH, BL,
	CH, CL,
	ALL
} phase;

void switch_phases(phase ph, phase pl);
void switch_ph(phase ph, switch_action state);
void switch_pl(phase ph, switch_action state);
void change_leading_vector(void);

#endif /* PHASES_H_ */
