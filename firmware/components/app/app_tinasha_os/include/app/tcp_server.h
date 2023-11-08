#ifndef APP_TINASHA_TCP_H
#define APP_TINASHA_TCP_H

#include "sys/socket.h"

typedef struct
{
    int sock_fd;
    int client_sock_fd;
    bool connected;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
} tcp_server_handle_t;

typedef struct
{
    int port;
} tcp_server_config_t;

tcp_server_handle_t tcp_server_setup(tcp_server_config_t config, uint32_t local_addr);
void tcp_server_listen(tcp_server_handle_t handle);
void tcp_server_shutdown(tcp_server_handle_t *handle);

// Client API
int tcp_server_find_client(tcp_server_handle_t *handle);
bool tcp_server_is_client_alive(tcp_server_handle_t *handle);
int tcp_server_ready_to_read(tcp_server_handle_t *handle);
void tcp_server_diconnect_client(tcp_server_handle_t *handle);
// Get data API
size_t tcp_server_receive_header(tcp_server_handle_t *handle, uint8_t *header);
size_t tcp_server_receive_data(tcp_server_handle_t *handle, uint8_t *data, size_t data_size);

void tcp_server_stop(tcp_server_handle_t handle);

#endif