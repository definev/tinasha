#ifndef VTS_UDP_H
#define VTS_UDP_H

#include "app/voice_to_server.h"

#include "freertos/FreeRTOS.h"

#define MAX_UDP_PACKET_SIZE 512
#define MAX_RETRY_COUNT 10

typedef struct
{
    uint32_t ip_addr;
    uint16_t port;
} voice_to_server_udp_config_t;

/**
 * @brief Callback function for handling UDP data received from the server.
 * 
 * This function is called when UDP data is received from the server. The data
 * is passed in as a void pointer along with its size. The function should
 * handle the data appropriately based on the application's requirements.
 * 
 * @param data Pointer to the received UDP data.
 * @param size Size of the received UDP data.
 */
void voice_to_server_udp_callback(void *data, size_t size);

void voice_to_server_udp_setup(voice_to_server_udp_config_t config);

void voice_to_server_udp_close();

#endif