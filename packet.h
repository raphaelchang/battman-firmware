#ifndef _PACKET_H_
#define _PACKET_H_

#include "ch.h"

void packet_process_byte(uint8_t byte);
void packet_send_packet(unsigned char *data, unsigned int len);
bool packet_connect_event(void);
void packet_timeout(void);

#endif /* _PACKET_H_ */
