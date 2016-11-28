#include "packet.h"
#include "ch.h"
#include "hal.h"
#include "comm_usb.h"
#include <string.h>
#include <stdio.h>
#include "console.h"
#include "config.h"
#include "datatypes.h"
#include "ltc6803.h"
#include "current_monitor.h"
#include "charger.h"
#include "analog.h"
#include "utils.h"
#include "fw_updater.h"
#include "stm32f30x_conf.h"
#include "faults.h"
#include "crc.h"

#define PACKET_TIMEOUT 1000
#define PACKET_START 'P'
#define PACKET_LONG_START 'Q'
#define PACKET_END '\n'

typedef enum
{
    START,
    LENGTH_HIGH,
    LENGTH_LOW,
    DATA,
    CRC_HIGH,
    CRC_LOW,
    END
} PacketState;

static uint16_t packet_index;
static uint8_t packet_buffer[2048];
static uint8_t packet_send_buffer[1024];
static bool connect_event = false;
static PacketState process_state = START;
static uint16_t process_timeout = PACKET_TIMEOUT;
static uint16_t packet_length = 0;
static uint16_t packet_crc = 0;

static void process_packet(unsigned char *data, unsigned int len);

void packet_process_byte(uint8_t byte)
{
    switch (process_state)
    {
        case START:
            if (byte == PACKET_START || byte == PACKET_LONG_START)
            {
                if (byte == PACKET_START)
                    process_state = LENGTH_LOW;
                else
                    process_state = LENGTH_HIGH;
                packet_index = 0;
                packet_length = 0;
                process_timeout = PACKET_TIMEOUT;
            }
            break;
        case LENGTH_HIGH:
            packet_length = byte << 8;
            process_state = LENGTH_LOW;
            process_timeout = PACKET_TIMEOUT;
            break;
        case LENGTH_LOW:
            packet_length |= byte;
            if (packet_length <= 0 || packet_length > 2048)
            {
                process_state = START;
            }
            process_state = DATA;
            process_timeout = PACKET_TIMEOUT;
            break;
        case DATA:
            packet_buffer[packet_index++] = byte;
            if (packet_index >= packet_length)
            {
                process_state = CRC_HIGH;
            }
            process_timeout = PACKET_TIMEOUT;
            break;
        case CRC_HIGH:
            packet_crc = byte << 8;
            process_state = CRC_LOW;
            process_timeout = PACKET_TIMEOUT;
            break;
        case CRC_LOW:
            packet_crc |= byte;
            process_state = END;
            process_timeout = PACKET_TIMEOUT;
            break;
        case END:
            if (byte == PACKET_END)
            {
                if (crc16(packet_buffer, packet_length) == (unsigned short)packet_crc)
                {
                    process_packet(packet_buffer, packet_length);
                }
                packet_index = 0;
                process_state = START;
                process_timeout = PACKET_TIMEOUT;
            }
            break;
        default:
            process_state = START;
            break;
    }
}

void packet_timeout(void)
{
    if (process_timeout > 0)
    {
        process_timeout--;
    }
    else
    {
        process_state = START;
    }
}

static void process_packet(unsigned char *data, unsigned int len)
{
    uint8_t id = data[0];
    data++;
    len--;
    uint32_t inx = 0;
    uint16_t res;
    uint32_t offset;
    uint16_t config_addr;
    uint8_t config_value[4];
    uint8_t readInx = 0;
    switch(id)
    {
        case PACKET_CONNECT:
            connect_event = true;
            packet_send_buffer[inx++] = PACKET_CONNECT;
            packet_send_buffer[inx++] = FW_VERSION_MAJOR + '0';
            packet_send_buffer[inx++] = '.';
            packet_send_buffer[inx++] = FW_VERSION_MINOR + '0';
            packet_send_packet((unsigned char*)packet_send_buffer, inx);
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
            packet_send_buffer[inx++] = faults_get_faults();
            packet_send_buffer[inx++] = charger_is_charging();
            packet_send_packet((unsigned char*)packet_send_buffer, inx);
            break;
        case PACKET_GET_CELLS:
            packet_send_buffer[inx++] = PACKET_GET_CELLS;
            float* cells = ltc6803_get_cell_voltages();
            for (uint8_t i = 0; i < config_get_configuration()->numCells; i++)
            {
                utils_append_float32(packet_send_buffer, cells[i], &inx);
            }
            packet_send_packet((unsigned char*)packet_send_buffer, inx);
            break;
        case PACKET_ERASE_NEW_FW:
            res = fw_updater_erase_new_firmware();
            packet_send_buffer[inx++] = PACKET_ERASE_NEW_FW;
            packet_send_buffer[inx++] = res == FLASH_COMPLETE ? 1 : 0;
            packet_send_packet((unsigned char*)packet_send_buffer, inx);
            break;
        case PACKET_WRITE_NEW_FW:
            offset = utils_parse_uint32(data, &inx);
            res = fw_updater_write_firmware(offset, data + inx, len - inx);
            inx = 0;
            packet_send_buffer[inx++] = PACKET_WRITE_NEW_FW;
            packet_send_buffer[inx++] = res == FLASH_COMPLETE ? 1 : 0;
            packet_send_packet((unsigned char*)packet_send_buffer, inx);
            break;
        case PACKET_JUMP_BOOTLOADER:
            fw_updater_jump_bootloader();
            break;
        case PACKET_CONFIG_SET_FIELD:
            config_addr = utils_parse_uint16(data, &inx);
            utils_reverse_copy(config_value, data + inx, len - inx);
            res = config_write_field(config_addr, config_value, len - inx);
            readInx = inx;
            inx = 0;
            packet_send_buffer[inx++] = PACKET_CONFIG_SET_FIELD;
            utils_append_uint16(packet_send_buffer, config_addr, &inx);
            utils_reverse_copy(packet_send_buffer + inx, config_value, len - readInx);
            inx += len - readInx;
            packet_send_packet((unsigned char*)packet_send_buffer, inx);
            break;
        case PACKET_CONFIG_GET_ALL:
            // Note: the config struct is sent in little endian
            packet_send_buffer[inx++] = PACKET_CONFIG_GET_ALL;
            memcpy(packet_send_buffer + inx, config_get_configuration(), sizeof(Config));
            inx += sizeof(Config);
            packet_send_packet((unsigned char*)packet_send_buffer, inx);
            break;
        default:
            break;
    }
}

void packet_send_packet(unsigned char *data, unsigned int len)
{
    unsigned char buffer[512];
    unsigned int inx = 0;
    if (len > 255)
    {
        buffer[inx++] = PACKET_LONG_START;
        buffer[inx++] = (uint8_t)(len >> 8);
        buffer[inx++] = (uint8_t)(len & 0xFF);
    }
    else
    {
        buffer[inx++] = PACKET_START;
        buffer[inx++] = (uint8_t)len;
    }
    memcpy(buffer + inx, data, len);
    inx += len;
    uint16_t crc = crc16(data, len);
    buffer[inx++] = (uint8_t)(crc >> 8);
    buffer[inx++] = (uint8_t)(crc & 0xFF);
    buffer[inx++] = PACKET_END;
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

