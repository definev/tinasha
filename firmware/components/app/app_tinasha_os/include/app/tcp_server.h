#ifndef APP_TINASHA_TCP_H
#define APP_TINASHA_TCP_H

#include "sys/socket.h"

typedef struct
{
    int sock_fd;
    int client_sock_fd;
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

// Client API
void tcp_server_find_client(tcp_server_handle_t *tcp_server_handle);
bool tcp_server_is_client_alive(tcp_server_handle_t *tcp_server_handle);
int tcp_server_ready_to_read(tcp_server_handle_t *tcp_server_handle);
// Get data API
size_t tcp_server_receive_header(tcp_server_handle_t *tcp_server_handle, char *header);
size_t tcp_server_receive_data(tcp_server_handle_t *tcp_server_handle, uint8_t *data, size_t data_size);

void tcp_server_stop(tcp_server_handle_t tcp_server_handle);

#endif