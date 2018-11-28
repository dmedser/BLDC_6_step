/* Author: t.me/dmedser */

#ifndef ISR_PRIORITIES_H_
#define ISR_PRIORITIES_H_

/* Service request priority number
 * (0 - lowest, 0xFF - highest priority)
 */

#define ISR_PN_RTOS_100_US               200
#define ISR_PN_RTOS_1_MS                 199
#define ISR_PN_RTOS_10_MS                197
#define ISR_PN_RTOS_100_MS               196
#define ISR_PN_CAN_RX                    100
#define ISR_PRIORITY_CORRECT_HALL_EVENT  198

#endif /* ISR_PRIORITIES_H_ */
