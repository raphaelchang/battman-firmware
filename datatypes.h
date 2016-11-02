#ifndef _DATATYPES_H_
#define _DATATYPES_H_

#include "ch.h"

typedef enum
{
    PACKET_CONNECT = 0x00,
    PACKET_CONSOLE = 0x01
} PacketID;

typedef enum
{
    EVENT_SWITCH = 0x00,
    EVENT_CHARGER = 0x01,
    EVENT_USB = 0x02,
    EVENT_RTCC = 0x03
} PowerOnEvent;

typedef struct
{
    volatile uint8_t CANDeviceID;
    volatile uint8_t numCells;
    volatile float lowVoltageCutoff;
    volatile float maxCurrentCutoff;
    volatile float chargeVoltage;
    volatile float chargeCurrent;
} Config;

#endif /* _DATATYPES_H_ */
