#include "console.h"
#include "packet.h"
#include "ch.h"
#include "hal.h"
#include "comm_usb.h"
#include "datatypes.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

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
    int len;
    static char print_buffer[255];

    print_buffer[0] = PACKET_CONSOLE;
    len = vsnprintf(print_buffer+1, 254, format, arg);
    va_end (arg);

    if(len > 0) {
        packet_send_packet((unsigned char*)print_buffer, (len<254)? len+1: 255);
    }
}
