#ifndef VTS_UDP_H
#define VTS_UDP_H

#include "app/voice_to_server.h"

#include "freertos/FreeRTOS.h"

#define MAX_UDP_PACKET_SIZE 512
#define MAX_RETRY_COUNT 10

typedef struct
{
    char *ip_addr;
    uint16_t port;
} voice_to_server_udp_config_t;

void voice_to_server_udp_callback(const char *data, size_t size);

void voice_to_server_udp_setup(voice_to_server_udp_config_t config);

void voice_to_server_udp_close();

#endif