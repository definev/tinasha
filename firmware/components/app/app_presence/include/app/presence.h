#ifndef APP_PRESENCE_H
#define APP_PRESENCE_H

#include "sys/socket.h"

typedef struct
{
    int socket;
    in_addr_t addr;
    uint16_t port;
    char tx_buffer[1460];
    size_t tx_buffer_len;
} presence_handle_t;

typedef struct
{
    in_addr_t addr;
    uint16_t port;
} presence_config_t;

presence_handle_t presence_activate(presence_config_t *config);

#endif