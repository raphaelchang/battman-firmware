#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "ch.h"

void console_process_command(char *command);
void console_printf(char* format, ...);

#endif /* _CONSOLE_H_ */
