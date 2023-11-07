#include "app/tcp_server.h"

#include "esp_log.h"
#include "sys/socket.h"

static const char *TAG = "tcp_server";

tcp_server_handle_t tcp_server_setup(tcp_server_config_t config, uint32_t local_addr)
{
    tcp_server_handle_t tcp_server_handle = {
        .sock_fd = -1,
        .local_addr = (struct sockaddr_in){
            .sin_family = AF_INET,
            .sin_port = htons(80),                // Port number 80 in network byte order
            .sin_addr.s_addr = htonl(local_addr), // IP address
        }};
    tcp_server_handle.sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (tcp_server_handle.sock_fd < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return tcp_server_handle;
    }
    if (bind(tcp_server_handle.sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr)) != 0)
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        return tcp_server_handle;
    }

    return tcp_server_handle;
}

int tcp_server_ready_to_read(tcp_server_handle_t *tcp_server_handle)
{
    if (tcp_server_handle->sock_fd < 0)
    {
        ESP_LOGE(TAG, "Socket not created");
        return -1;
    }
    static int bytes_available = 0;
    ioctl(tcp_server_handle->sock_fd, FIONREAD, &bytes_available);
    return bytes_available;
}

bool tcp_server_is_client_alive(tcp_server_handle_t *tcp_server_handle)
{
    if (tcp_server_handle->connected)
    {
        uint8_t dummy;
        int res = recv(tcp_server_handle->sock_fd, &dummy, 0, MSG_DONTWAIT);
        // avoid unused var warning by gcc
        (void)res;
        // recv only sets errno if res is <= 0
        if (res <= 0)
        {
            switch (errno)
            {
            case EWOULDBLOCK:
            case ENOENT: // caused by vfs
                tcp_server_handle->connected = true;
                break;
            case ENOTCONN:
            case EPIPE:
            case ECONNRESET:
            case ECONNREFUSED:
            case ECONNABORTED:
                tcp_server_handle->connected = false;
                ESP_LOGI(TAG, "Disconnected: RES: %d, ERR: %d", res, errno);
                break;
            default:
                tcp_server_handle->connected = true;
                ESP_LOGI(TAG, "Unexpected: RES: %d, ERR: %d", res, errno);
                break;
            }
        }
        else
        {
            tcp_server_handle->connected = true;
        }
    }
    return tcp_server_handle->connected;
}

void tcp_server_listen(tcp_server_handle_t tcp_server_handle)
{
    if (listen(tcp_server_handle.sock_fd, 2) < 0)
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        return;
    }
}

void tcp_server_stop(tcp_server_handle_t tcp_server_handle)
{
    close(tcp_server_handle.sock_fd);
}

void tcp_server_find_client(tcp_server_handle_t *tcp_server_handle)
{
    tcp_server_handle->client_sock_fd = accept(
        tcp_server_handle->sock_fd,
        (struct sockaddr *)&tcp_server_handle->remote_addr,
        sizeof(tcp_server_handle->remote_addr));
}

size_t tcp_server_receive_header(tcp_server_handle_t *tcp_server_handle, uint8_t *header)
{
    return tcp_server_receive_data(tcp_server_handle, (uint8_t *)header, 6);
}

size_t tcp_server_receive_data(tcp_server_handle_t *tcp_server_handle, uint8_t *data, size_t data_size)
{
    return recv(tcp_server_handle->client_sock_fd, data, data_size, MSG_DONTWAIT);
}
