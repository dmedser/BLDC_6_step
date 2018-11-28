/* Author: t.me/dmedser */

#include "scu_clk_cfg.h"
#include <IfxScuCcu.h>

/* fPLL = 200 MHz */
void scu_clk_init(void) {
	/* Default configuration for the PLL initial steps */
	static const IfxScuCcu_PllStepsConfig IfxScuCcu_defaultPllConfigSteps[] = {
	    IFXSCU_CFG_PLL_STEPS
	};

	/* Default configuration for the Clock Configuration */
	IfxScuCcu_Config IfxScuCcu_defaultClockConfig = {
		{
			sizeof(IfxScuCcu_defaultPllConfigSteps) / sizeof(IfxScuCcu_PllStepsConfig),
			(IfxScuCcu_PllStepsConfig *)IfxScuCcu_defaultPllConfigSteps,
			IFXSCU_CFG_PLL_INITIAL_STEP
		},
		IFXSCU_CFG_CLK_DISTRIBUTION,
		IFXSCU_CFG_FLASH_WAITSTATE
	};

	/* Standard PLL initialization */
	IfxScuCcu_init(&IfxScuCcu_defaultClockConfig);

}

