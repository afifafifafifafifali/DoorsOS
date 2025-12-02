#ifndef RTL8139_H
#define RTL8139_H

#include <stdint.h>

void rtl8139_init(void);
int rtl8139_send_packet(void* data, uint32_t len);

#endif
