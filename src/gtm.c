/* Author: t.me/dmedser */

#include "gtm.h"
#include <IfxGtm.h>
#include <IfxGtm_Cmu.h>

void gtm_init(void) {
	Ifx_GTM *gtm = &MODULE_GTM;

	IfxGtm_enable(gtm);

	/* fGTM = (fSRC / GTMDIV)
	 * fSRC   by default = fPLL = IFX_CFG_SCU_PLL_FREQUENCY = 200 MHz
	 * GTMDIV by default = 2
	 */

	/* Set GTMDIV = 1, fGTM = fPLL = 200 MHz */
	uint16_t password = IfxScuWdt_getSafetyWatchdogPassword();

	Ifx_SCU_CCUCON1 ccucon1 = SCU_CCUCON1;

	/* Disable TRAP for SMU (oscillator watchdog and unlock detection) */
	IfxScuWdt_clearSafetyEndinit(password);

	ccucon1.B.GTMDIV = 1U;
	ccucon1.B.UP     = 1U;
	SCU_CCUCON1.U    = ccucon1.U;

	IfxScuWdt_setSafetyEndinit(password);
}
