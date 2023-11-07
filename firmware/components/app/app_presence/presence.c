#include "app/presence.h"

#include "sys/socket.h"

esp_err_t presence_start(presence_handle_t *handle, presence_config_t *config)
{

    if ((handle->socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        log_e("could not create socket: %d", errno);
        return 0;
    }

    int yes = 1;
    if (setsockopt(handle->socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        log_e("could not set socket option: %d", errno);
        stop();
        return 0;
    }

    struct sockaddr_in addr;
    memset((char *)&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(handle->port);
    addr.sin_addr.s_addr = (uint32_t)handle->addr;
    if (bind(handle->socket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        log_e("could not bind socket: %d", errno);
        stop();
        return 0;
    }
    fcntl(handle->socket, F_SETFL, O_NONBLOCK);
    return 1;
}

esp_err_t presence_push(presence_handle_t *handle, const char *data, size_t len)
{
    if (handle->tx_buffer_len + len > sizeof(handle->tx_buffer))
    {
        log_e("tx buffer overflow");
        return 0;
    }
    memcpy(handle->tx_buffer + handle->tx_buffer_len, data, len);
    handle->tx_buffer_len += len;
    return 1;
}

esp_err_t presence_stop(presence_handle_t *handle)
{
    struct sockaddr_in recipient;
    recipient.sin_addr.s_addr = (uint32_t)handle->addr;
    recipient.sin_family = AF_INET;
    recipient.sin_port = htons(handle->port);
    int sent = sendto(handle->socket, handle->tx_buffer, handle->tx_buffer_len, 0, (struct sockaddr *)&recipient, sizeof(recipient));
    if (sent < 0)
    {
        log_e("could not send data: %d", errno);
        return 0;
    }
    return 1;
}

presence_handle_t presence_activate(presence_config_t *config)
{
    presence_handle_t handle;

    presence_start(&handle, &config);
    presence_push(&handle, CONFIG_APP_NAME, sizeof(CONFIG_APP_NAME));
    presence_stop(&handle);

    return handle;
}