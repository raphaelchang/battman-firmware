#include "comm_can.h"
#include "ch.h"
#include "hal.h"
#include "hw_conf.h"
#include "datatypes.h"
#include <string.h>
#include "config.h"
#include "utils.h"

#define RX_FRAMES_SIZE  100

static const CANConfig cancfg = {
    CAN_MCR_ABOM | CAN_MCR_AWUM | CAN_MCR_TXFP,
    CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
    CAN_BTR_TS1(8) | CAN_BTR_BRP(5)
};
static THD_WORKING_AREA(can_read_thread_wa, 512);
static THD_WORKING_AREA(can_process_thread_wa, 4096);
static THD_FUNCTION(can_read_thread, arg);
static THD_FUNCTION(can_process_thread, arg);

static CANRxFrame rx_frames[RX_FRAMES_SIZE];
static int rx_frame_read;
static int rx_frame_write;
static thread_t *process_tp;
static mutex_t can_mtx;
static volatile Config *config;

void comm_can_init(void)
{
    config = config_get_configuration();
    chMtxObjectInit(&can_mtx);
    canStart(&CAND1, &cancfg);
    chThdCreateStatic(can_read_thread_wa, sizeof(can_read_thread_wa), NORMALPRIO + 1, can_read_thread, NULL);
    chThdCreateStatic(can_process_thread_wa, sizeof(can_process_thread_wa), NORMALPRIO, can_process_thread, NULL);
}

static float infinity_current = 99;
static THD_FUNCTION(can_read_thread, arg) {
    (void)arg;
    chRegSetThreadName("CAN read");

    event_listener_t el;
    CANRxFrame rxmsg;

    chEvtRegister(&CAND1.rxfull_event, &el, 0);

    while(!chThdShouldTerminateX()) {
        if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(10)) == 0) {
            continue;
        }

        msg_t result = canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE);

        while (result == MSG_OK) {
            rx_frames[rx_frame_write++] = rxmsg;
            if (rx_frame_write == RX_FRAMES_SIZE) {
                rx_frame_write = 0;
            }

            chEvtSignal(process_tp, (eventmask_t) 1);

            result = canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE);
        }
    }

    chEvtUnregister(&CAND1.rxfull_event, &el);
}

static THD_FUNCTION(can_process_thread, arg) {
    (void)arg;

    chRegSetThreadName("CAN process");
    process_tp = chThdGetSelfX();

    int32_t ind = 0;

    for(;;) {
	chEvtWaitAny((eventmask_t) 1);

	static int32_t ind = 0;
	while (rx_frame_read != rx_frame_write) {
	    CANRxFrame rxmsg = rx_frames[rx_frame_read++];

	    if (rxmsg.IDE == CAN_IDE_EXT) {
		uint8_t sender = rxmsg.EID & 0xFF;
		uint8_t receiver = (rxmsg.EID >> 8) & 0xFF;
		CANPacketID id = rxmsg.EID >> 16;
		if ((receiver == CAN_BROADCAST || receiver == config->CANDeviceID) && sender != config->CANDeviceID) {
		    switch (id) {
			case CAN_PACKET_INFINITY_SET_CURRENT:
			    break;
			case CAN_PACKET_INFINITY_STATUS:
                            ind = 0;
                            float rpm = (float)utils_parse_float32(rxmsg.data8, &ind);
                            float current = (float)utils_parse_float32(rxmsg.data8, &ind);
                            infinity_current = current;
			    break;
			default:
			    break;
		    }
		}
	    }
	    if (rx_frame_read == RX_FRAMES_SIZE) {
		rx_frame_read = 0;
	    }
	}
    }
}

float comm_can_get_infinity_current(void)
{
    return (float)infinity_current;
}

void comm_can_update(void)
{
}

void comm_can_transmit(uint8_t receiver, CANPacketID packetID, uint8_t *data, uint8_t len)
{
    CANTxFrame txmsg;
    txmsg.IDE = CAN_IDE_EXT;
    txmsg.EID = config->CANDeviceID | (receiver << 8) | (packetID << 16);
    txmsg.RTR = CAN_RTR_DATA;
    txmsg.DLC = len;
    memcpy(txmsg.data8, data, len);

    chMtxLock(&can_mtx);
    canTransmit(&CAND1, CAN_ANY_MAILBOX, &txmsg, MS2ST(20));
    chMtxUnlock(&can_mtx);
}

