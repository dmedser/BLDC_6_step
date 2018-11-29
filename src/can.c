/* Author: t.me/dmedser */

#include "can.h"
#include "isr_priorities.h"
#include "roll_params.h"
#include "rotor.h"
#include "pwm.h"
#include "rtos.h"
#include <IfxMultican_Can.h>
#include <IfxSrc_reg.h>
#include <stdint.h>
#include <Platform_Types.h>
#include <machine/cint.h>

#define CAN_BAUDRATE_BPS        500000  /* 500 kBit/sec */
#define CAN_RX_PIN              &IfxMultican_RXD0A_P02_1_IN
#define CAN_TX_PIN              &IfxMultican_TXD0_P02_0_OUT

#define CAN_DST_MO_MSG_ID       ROLLS_CAN_ID

/* CAN handle */
static IfxMultican_Can can;

/* Node handle */
static IfxMultican_Can_Node can_node;

/* Destination Message Object handle */
static IfxMultican_Can_MsgObj can_dst_mo;

void can_init(void) {

	/* CAN */
	IfxMultican_Can_Config can_cfg;

	/* Default settings */
	IfxMultican_Can_initModuleConfig(&can_cfg, &MODULE_CAN);

	IfxMultican_Can_initModule(&can, &can_cfg);

	/* CAN node */
	IfxMultican_Can_NodeConfig can_node_cfg;

	/* Default settings */
	IfxMultican_Can_Node_initConfig(&can_node_cfg, &can);

	can_node_cfg.baudrate   = CAN_BAUDRATE_BPS;
	can_node_cfg.nodeId     = IfxMultican_NodeId_0;
	can_node_cfg.rxPin      = CAN_RX_PIN;
	can_node_cfg.txPin      = CAN_TX_PIN;
	can_node_cfg.rxPinMode  = IfxPort_InputMode_pullUp;
	can_node_cfg.txPinMode  = IfxPort_OutputMode_pushPull;

	IfxMultican_Can_Node_init(&can_node, &can_node_cfg);

	/* Destination MO, RX */
	{
		IfxMultican_Can_MsgObjConfig can_dst_mo_cfg;

		/* Default settings */
		IfxMultican_Can_MsgObj_initConfig(&can_dst_mo_cfg, &can_node);

		can_dst_mo_cfg.msgObjId             = 1;
		can_dst_mo_cfg.messageId            = CAN_DST_MO_MSG_ID;
		can_dst_mo_cfg.frame                = IfxMultican_Frame_receive;
		can_dst_mo_cfg.rxInterrupt.srcId    = IfxMultican_SrcId_1;
		can_dst_mo_cfg.rxInterrupt.enabled  = TRUE;

		IfxMultican_Can_MsgObj_init(&can_dst_mo, &can_dst_mo_cfg);
	}

	/* CAN receive interrupt */
	MODULE_SRC.CAN.CAN[0].INT[1].B.SRPN = ISR_PN_CAN_RX;
	MODULE_SRC.CAN.CAN[0].INT[1].B.SRE  = 0b1;
	_install_int_handler(ISR_PN_CAN_RX, (void (*) (int))ISR_can_rx, 0);

}


static uint32_t swap_endianness(uint32_t value) {
	return ((value & ((uint32_t)0xFF << 0))  << 24) |
	       ((value & ((uint32_t)0xFF << 8))  << 8)  |
	       ((value & ((uint32_t)0xFF << 16)) >> 8)  |
	       ((value & ((uint32_t)0xFF << 24)) >> 24);
}


void ISR_can_rx(void) {
	IfxCpu_forceDisableInterrupts();

	IfxMultican_Message rx_msg;

	IfxMultican_Can_MsgObj_readMessage(&can_dst_mo, &rx_msg);

	uint32_t data_high = swap_endianness(rx_msg.data[1]);
	uint32_t data_low  = swap_endianness(rx_msg.data[0]);

	uint8_t received_opcode = ((uint8_t)data_high) & 0x01;

	#define OPCODE_WRITE 1
	#define OPCODE_READ  0

	if((rx_msg.id == CAN_DST_MO_MSG_ID) && (received_opcode == OPCODE_WRITE)) {

		uint16_t received_roll_no = (uint16_t)(data_low >> 16);

		if(received_roll_no == ROLL_NO) {

			rotor.target_turns_per_min = (uint16_t)data_low;

			float coarse_duty_cycle_preset = (rotor.target_turns_per_min > 0) ?
			((rotor.target_turns_per_min - rotor.calibration_offset) / rotor.calibration_multiplier) : 0;

			update_duty_cycle((coarse_duty_cycle_preset > 0) ? coarse_duty_cycle_preset : 0);

			last_can_cmd_timestamp = cnt_100_ms;
		}

	}

	IfxCpu_enableInterrupts();
}

