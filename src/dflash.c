/* Author: t.me/dmedser */

#include "dflash.h"
#include <IfxFlash.h>
#include <IfxFlash_cfg.h>
#include "rotor.h"

#define DFLASH_PAGE_SIZE                        sizeof(uint64_t)
#define DFLASH_ADDRESS(START_ADDRESS, OFFSET)   (START_ADDRESS + (OFFSET * DFLASH_PAGE_SIZE))
#define CALIBRATION_MULTIPLIER_DFLASH_ADDRESS   DFLASH_ADDRESS(IFXFLASH_DFLASH_START, 0)
#define CALIBRATION_OFFSET_DFLASH_ADDRESS       DFLASH_ADDRESS(IFXFLASH_DFLASH_START, 1)

/* Writes 64-bit word to an data flash absolute address (8-byte aligned) */
static void dflash_wr_u64(uint32_t addr, uint64_t data) {

	IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

	IfxFlash_erasePhysicalSector(addr);
	IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

	IfxFlash_enterPageMode(addr);
	IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

	uint32_t data_low  = (uint32_t)data;
	uint32_t data_high = (uint32_t)(data >> 32);

	IfxFlash_loadPage(addr, data_low, data_high);
	IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

	uint16_t password = IfxScuWdt_getSafetyWatchdogPassword();

	IfxScuWdt_clearSafetyEndinit(password);

	IfxFlash_writePage(addr);
	IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);

	IfxScuWdt_setSafetyEndinit(password);

	IfxFlash_resetToRead(0);
	IfxFlash_waitUnbusy(0, IfxFlash_FlashType_D0);
}

static uint64_t dflash_rd_u64(uint32_t addr) {
	return (uint64_t)*((uint32_t *)addr);
}

static void dflash_wr_f(uint32_t addr, float data) {
	uint32_t *ptr_u32 = (uint32_t *)&data;
	dflash_wr_u64(addr, *ptr_u32);
}

static float dflash_rd_f(uint32_t addr) {
	uint64_t read_res = dflash_rd_u64(addr);
	float *ptr_f = (float *)&read_res;
	return *ptr_f;
}

void upload_into_dflash(parameter_idx parameter) {
	switch(parameter){
	case CALIBRATION_MULTIPLIER:
		dflash_wr_f(CALIBRATION_MULTIPLIER_DFLASH_ADDRESS, rotor.calibration_multiplier);
		break;
	case CALIBRATION_OFFSET:
		dflash_wr_f(CALIBRATION_OFFSET_DFLASH_ADDRESS, rotor.calibration_offset);
		break;
	}
}

void download_from_dflash(parameter_idx parameter) {
	switch(parameter){
	case CALIBRATION_MULTIPLIER:
		rotor.calibration_multiplier = dflash_rd_f(CALIBRATION_MULTIPLIER_DFLASH_ADDRESS);
		break;
	case CALIBRATION_OFFSET:
		rotor.calibration_offset = dflash_rd_f(CALIBRATION_OFFSET_DFLASH_ADDRESS);
		break;
	}
}
