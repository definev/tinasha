#include "os/tcp_server.h"

#include "esp_log.h"
#include "sys/socket.h"

static const char *TAG = "tcp_server";

tcp_server_handle_t tcp_server_setup(uint16_t port)
{
    tcp_server_handle_t handle = {
        .sock_fd = -1,
        .local_addr = (struct sockaddr_in){
            .sin_family = AF_INET,
            .sin_port = htons(port),              // Port number 80 in network byte order
            .sin_addr.s_addr = htons(INADDR_ANY), // IP address
        }};

    handle.sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (handle.sock_fd < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %s", strerror(errno));
        goto CLEAN_UP;
    }

    int opt = 1;
    setsockopt(handle.sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(handle.sock_fd, (struct sockaddr *)&handle.local_addr, sizeof(handle.local_addr)) != 0)
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %s", strerror(errno));
        goto CLEAN_UP;
    }
    if (listen(handle.sock_fd, 1) < 0)
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d, %s", errno, strerror(errno));
        goto CLEAN_UP;
    }

    ESP_LOGI(TAG, "Socket %d created, local_addr: %s", handle.sock_fd, inet_ntoa(handle.local_addr.sin_addr));

    return handle;

CLEAN_UP:
{
    if (handle.sock_fd >= 0)
    {
        close(handle.sock_fd);
    }
    return handle;
}
}

int tcp_server_ready_to_read(tcp_server_handle_t *handle)
{
    if (handle->client_sock_fd < 0)
    {
        ESP_LOGE(TAG, "Socket not created");
        return -1;
    }
    static int bytes_available = 0;
    ioctl(handle->client_sock_fd, FIONREAD, &bytes_available);
    return bytes_available;
}

void tcp_server_diconnect_client(tcp_server_handle_t *handle)
{
    if (handle->client_sock_fd >= 0)
    {
        close(handle->client_sock_fd);
        handle->client_sock_fd = -1;
    }
}

bool tcp_server_is_client_alive(tcp_server_handle_t *handle)
{
    handle->connected = (handle->client_sock_fd >= 0);
    return handle->connected;
}

void tcp_server_listen(tcp_server_handle_t handle)
{
    if (listen(handle.sock_fd, 2) < 0)
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        return;
    }
}

void tcp_server_shutdown(tcp_server_handle_t *handle)
{
}

void tcp_server_stop(tcp_server_handle_t handle)
{
    close(handle.sock_fd);
}

int keepAlive = 1;
int keepIdle = 10;
int keepInterval = 10;
int keepCount = 5;

int tcp_server_find_client(tcp_server_handle_t *handle)
{
    socklen_t addr_len = sizeof(handle->remote_addr);
    handle->client_sock_fd = accept(
        handle->sock_fd,
        (struct sockaddr *)&handle->remote_addr,
        &addr_len);
    if (handle->client_sock_fd < 0)
    {
        ESP_LOGE(TAG, "Unable to accept connection: errno %s", strerror(errno));
        return -1;
    }

    // Set tcp keepalive option
    setsockopt(handle->client_sock_fd, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
    setsockopt(handle->client_sock_fd, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
    setsockopt(handle->client_sock_fd, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
    setsockopt(handle->client_sock_fd, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));

    fcntl(handle->client_sock_fd, F_GETFL, O_NONBLOCK);
    // Convert ip address to string
    ESP_LOGI(TAG, "Socket accepted ip address: %s", inet_ntoa(handle->remote_addr.sin_addr));

    return handle->client_sock_fd;
}

size_t tcp_server_receive_header(tcp_server_handle_t *handle, uint8_t *header)
{
    return tcp_server_receive_data(handle, (uint8_t *)header, 6);
}

size_t tcp_server_receive_data(tcp_server_handle_t *handle, uint8_t *data, size_t data_size)
{
    return recv(handle->client_sock_fd, data, data_size, MSG_WAITALL);
}
