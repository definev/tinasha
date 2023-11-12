#ifndef APP_TINASHA_UDP_SERVER_H
#define APP_TINASHA_UDP_SERVER_H

#include "freertos/FreeRTOS.h"

int udp_server_begin_packet(char *ip, uint16_t port);

void udp_server_write(const char *buffer, size_t size);

void udp_server_end_packet();

#endif