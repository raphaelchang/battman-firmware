#include "console.h"
#include "packet.h"
#include "ch.h"
#include "hal.h"
#include "comm_usb.h"
#include "datatypes.h"
#include <string.h>
#include <stdio.h>
#include <chprintf.h>
#include "memstreams.h"
#include "config.h"
#include "ltc6803.h"
#include "comm_can.h"

void console_process_command(char *command)
{
    enum { kMaxArgs = 64 };
    int argc = 0;
    char *argv[kMaxArgs];

    char *p2 = strtok(command, " ");
    while (p2 && argc < kMaxArgs) {
        argv[argc++] = p2;
        p2 = strtok(0, " ");
    }
    if (argc == 0) {
        console_printf("No command received\n");
        console_printf("\r\n");
        return;
    }
    if (strcmp(argv[0], "ping") == 0) {
        console_printf("pong\n");
        console_printf("\r\n");
    }
    else if (strcmp(argv[0], "mem") == 0) {
        size_t n, size;
        n = chHeapStatus(NULL, &size);
        console_printf("core free memory : %u bytes\n", chCoreGetStatusX());
        console_printf("heap fragments   : %u\n", n);
        console_printf("heap free total  : %u bytes\n", size);
        console_printf("\r\n");
    }
    else if (strcmp(argv[0], "threads") == 0) {
        thread_t *tp;
        static const char *states[] = {CH_STATE_NAMES};
        console_printf("    addr    stack prio refs     state           name time    \n");
        console_printf("-------------------------------------------------------------\n");
        tp = chRegFirstThread();
        do {
            console_printf("%.8lx %.8lx %4lu %4lu %9s %14s %lu\n",
                    (uint32_t)tp, (uint32_t)tp->p_ctx.r13,
                    (uint32_t)tp->p_prio, (uint32_t)(tp->p_refs - 1),
                    states[tp->p_state], tp->p_name, (uint32_t)tp->p_time);
            tp = chRegNextThread(tp);
        } while (tp != NULL);
        console_printf("\r\n");
    }
    else if (strcmp(argv[0], "uptime") == 0) {
        console_printf("System uptime: %d seconds\n", ST2S(chVTGetSystemTime()));
        console_printf("\r\n");
    }
    else if (strcmp(argv[0], "cell_voltages") == 0) {
        Config *config = config_get_configuration();
        uint16_t *cells = ltc6803_get_cell_voltages();
        for (uint8_t i = 0; i < config->numCells; i++)
        {
            console_printf("Cell %d: %fmV\n", i + 1, cells[i] * 1.5);
        }
        console_printf("\r\n");
    }
    else if (strcmp(argv[0], "infinity_current") == 0) {
        console_printf("Current: %f\n", comm_can_get_infinity_current());
        console_printf("\r\n");
    }
    else
    {
        console_printf("%s: command not found\n", argv[0]);
        // console_printf("type help for a list of available commands\n");
        console_printf("\r\n");
    }
}
void console_printf(char* format, ...) {
    va_list arg;
    va_start (arg, format);
    unsigned int len;
    static char print_buffer[255];

    print_buffer[0] = PACKET_CONSOLE;
    len = vsnprintf(print_buffer+1, 254, format, arg);

    /*MemoryStream ms;*/
    /*BaseSequentialStream *chp;*/
    /*size_t size = 254;*/
    /*size_t size_wo_nul;*/

    /*if (size > 0)*/
	/*size_wo_nul = size - 1;*/
    /*else*/
	/*size_wo_nul = 0;*/

    /* Memory stream object to be used as a string writer, reserving one
       byte for the final zero.*/
    /*msObjectInit(&ms, (uint8_t *)(print_buffer + 1), size_wo_nul, 0);*/

    /*[> Performing the print operation using the common code.<]*/
    /*chp = (BaseSequentialStream *)(void *)&ms;*/
    /*len = chvprintf(chp, format, arg);*/
    va_end (arg);
    /*if (ms.eos < size)*/
        /*print_buffer[ms.eos + 1] = 0;*/

    if(len > 0) {
        packet_send_packet((unsigned char*)print_buffer, (len<254) ? len+1: 255);
    }
}
