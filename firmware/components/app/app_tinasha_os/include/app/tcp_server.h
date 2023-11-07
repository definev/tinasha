#ifndef APP_TINASHA_TCP_H
#define APP_TINASHA_TCP_H

#include "sys/socket.h"

typedef struct
{
    int sock_fd;
    bool connected;
    struct sockaddr_storage local_addr;
    struct sockaddr_storage remote_addr;
} tcp_server_handle_t;

typedef struct
{
    int port;
} tcp_server_config_t;

tcp_server_handle_t tcp_server_setup(tcp_server_config_t config, struct sockaddr_storage local_addr);
void tcp_server_listen(tcp_server_handle_t tcp_server_handle);
bool tcp_server_connection(tcp_server_handle_t *tcp_server_handle);
void tcp_server_stop(tcp_server_handle_t tcp_server_handle);
int tcp_server_available_bytes(tcp_server_handle_t tcp_server_handle);

#endif