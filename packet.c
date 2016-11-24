#include "packet.h"
#include "ch.h"
#include "hal.h"
#include "comm_usb.h"
#include <string.h>
#include <stdio.h>
#include "console.h"
#include "datatypes.h"
#include "current_monitor.h"
#include "charger.h"
#include "analog.h"
#include "utils.h"

static uint16_t packet_index;
static uint8_t packet_buffer[2048];
static uint8_t packet_send_buffer[1024];
static bool in_packet = false;
static bool connect_event = false;

static void process_packet(unsigned char *data, unsigned int len);

void packet_process_byte(uint8_t byte)
{
    if (in_packet)
    {
        if (byte == '\n')
        {
            in_packet = false;
            process_packet(packet_buffer, packet_index);
            packet_index = 0;
        }
        else if (packet_index < 2048)
        {
            packet_buffer[packet_index++] = byte;
        }
    }
    else if (byte == 'P')
    {
        in_packet = true;
        packet_index = 0;
    }
}

static void process_packet(unsigned char *data, unsigned int len)
{
    uint8_t id = data[0];
    data++;
    len--;
    uint32_t inx = 0;
    switch(id)
    {
        case PACKET_CONNECT:
            connect_event = true;
            break;
        case PACKET_CONSOLE:
            data[len] = '\0';
            console_process_command(data);
            break;
        case PACKET_GET_DATA:
            packet_send_buffer[inx++] = PACKET_GET_DATA;
            utils_append_float32(packet_send_buffer, current_monitor_get_bus_voltage(), &inx);
            utils_append_float32(packet_send_buffer, analog_temperature(), &inx);
            utils_append_float32(packet_send_buffer, current_monitor_get_current(), &inx);
            utils_append_float32(packet_send_buffer, charger_get_output_voltage(), &inx);
            packet_send_buffer[inx++] = '\n';
            packet_send_packet((unsigned char*)packet_send_buffer, inx);
        default:
            break;
    }
}

void packet_send_packet(unsigned char *data, unsigned int len)
{
    unsigned char buffer[512];
    unsigned int inx = 0;
    buffer[inx++] = 'P';
    memcpy(buffer + inx, data, len);
    inx += len;
    comm_usb_send(buffer, inx);
}

bool packet_connect_event(void)
{
    if (connect_event)
    {
        connect_event = false;
        return true;
    }
    return false;
}

