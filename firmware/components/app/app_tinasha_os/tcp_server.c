#include "app/tcp_server.h"

#include "esp_log.h"
#include "sys/socket.h"

static const char *TAG = "tcp_server";

tcp_server_handle_t tcp_server_setup(tcp_server_config_t config, uint32_t local_addr)
{
    tcp_server_handle_t handle = {
        .sock_fd = -1,
        .local_addr = (struct sockaddr_in){
            .sin_family = AF_INET,
            .sin_port = htons(config.port),       // Port number 80 in network byte order
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
    if (handle->sock_fd < 0)
    {
        ESP_LOGE(TAG, "Socket not created");
        return -1;
    }
    static int bytes_available = 0;
    ioctl(handle->sock_fd, FIONREAD, &bytes_available);
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
    if (handle->connected)
    {
        uint8_t dummy;
        int res = recv(handle->sock_fd, &dummy, 0, MSG_DONTWAIT);
        // avoid unused var warning by gcc
        (void)res;
        // recv only sets errno if res is <= 0
        if (res <= 0)
        {
            switch (errno)
            {
            case EWOULDBLOCK:
            case ENOENT: // caused by vfs
                handle->connected = true;
                break;
            case ENOTCONN:
            case EPIPE:
            case ECONNRESET:
            case ECONNREFUSED:
            case ECONNABORTED:
                handle->connected = false;
                ESP_LOGI(TAG, "Disconnected: RES: %d, ERR: %d", res, errno);
                break;
            default:
                handle->connected = true;
                ESP_LOGI(TAG, "Unexpected: RES: %d, ERR: %d", res, errno);
                break;
            }
        }
        else
        {
            handle->connected = true;
        }
    }
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
int keepIdle = 5;
int keepInterval = 5;
int keepCount = 3;

int tcp_server_find_client(tcp_server_handle_t *handle)
{
    socklen_t addr_len = sizeof(handle->remote_addr);
    int client_sock = accept(
        handle->sock_fd,
        (struct sockaddr *)&handle->remote_addr,
        &addr_len);
    if (client_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to accept connection: errno %s", strerror(errno));
        return -1;
    }

    handle->client_sock_fd = client_sock;
    // Set tcp keepalive option
    setsockopt(client_sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
    setsockopt(client_sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
    setsockopt(client_sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
    setsockopt(client_sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));

    // Convert ip address to string
    ESP_LOGI(TAG, "Socket accepted ip address: %s", inet_ntoa(handle->remote_addr.sin_addr));
    return client_sock;
}

size_t tcp_server_receive_header(tcp_server_handle_t *handle, uint8_t *header)
{
    return tcp_server_receive_data(handle, (uint8_t *)header, 6);
}

size_t tcp_server_receive_data(tcp_server_handle_t *handle, uint8_t *data, size_t data_size)
{
    return recv(handle->client_sock_fd, data, data_size, MSG_DONTWAIT);
}
