/* Author: t.me/dmedser */

#include "ports.h"
#include <IfxPort_Io.h>
#include <IfxPort_PinMap.h>
#include <IfxGtm_PinMap.h>
#include <IfxCcu6.h>

void ports_init(void) {
	/* PH_A_H */
	IfxGtm_PinMap_setTomTout(&IfxGtm_TOM0_12_TOUT4_P02_4_OUT, IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
	/* PH_B_H */
	IfxGtm_PinMap_setTomTout(&IfxGtm_TOM0_13_TOUT5_P02_5_OUT, IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);
	/* PH_C_H */
	IfxGtm_PinMap_setTomTout(&IfxGtm_TOM0_11_TOUT3_P02_3_OUT, IfxPort_OutputMode_pushPull, IfxPort_PadDriver_cmosAutomotiveSpeed1);

	const IfxPort_Io_ConfigPin phases_low[3] = {
		/* PH_A_L */
		{&IfxPort_P11_3, IfxPort_Mode_outputPushPullGeneral, IfxPort_PadDriver_cmosAutomotiveSpeed1},
		/* PH_B_L */
		{&IfxPort_P11_6, IfxPort_Mode_outputPushPullGeneral, IfxPort_PadDriver_cmosAutomotiveSpeed1},
		/* PH_C_L */
		{&IfxPort_P11_9, IfxPort_Mode_outputPushPullGeneral, IfxPort_PadDriver_cmosAutomotiveSpeed1}
	};

	const IfxPort_Io_Config phases_low_cfg = {
		sizeof(phases_low) / sizeof(IfxPort_Io_ConfigPin),
		(IfxPort_Io_ConfigPin *)phases_low
	};

	IfxPort_Io_initModule(&phases_low_cfg);

	IfxPort_setPinLow(PH_A_L);
	IfxPort_setPinLow(PH_B_L);
	IfxPort_setPinLow(PH_C_L);

	/* Hall Sensor 0 */
	IfxCcu6_initCcpos0Pin(&IfxCcu60_CCPOS0A_P02_6_IN, IfxPort_InputMode_noPullDevice);
	/* Hall Sensor 1 */
	IfxCcu6_initCcpos0Pin(&IfxCcu60_CCPOS1A_P02_7_IN, IfxPort_InputMode_noPullDevice);
	/* Hall Sensor 2 */
	IfxCcu6_initCcpos0Pin(&IfxCcu60_CCPOS2A_P02_8_IN, IfxPort_InputMode_noPullDevice);
}
