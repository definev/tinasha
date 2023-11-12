#include "sys/socket.h"

#include "esp_log.h"

static const char *TAG = "udp_server";

static int udp_server = -1;
static char *tx_buffer = NULL;
static size_t tx_buffer_len = 0;

static struct sockaddr_in recipient;

void _stop()
{
    if (tx_buffer)
    {
        tx_buffer = NULL;
    }
    tx_buffer_len = 0;
    if (udp_server == -1)
        return;
    close(udp_server);
    udp_server = -1;
}

static char *remote_ip = NULL;
static uint16_t remote_port = 0;
void _begin_packet()
{
    if (!remote_port)
        return;

    // allocate tx_buffer if is necessary
    if (!tx_buffer)
    {
        tx_buffer = (char *)malloc(1460);
        if (!tx_buffer)
        {
            ESP_LOGE(TAG, "could not create tx buffer: %d", errno);
            return;
        }
    }
    tx_buffer_len = 0;

    // check whereas socket is already open
    if (udp_server != -1)
        return;

    if ((udp_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) == -1)
    {
        ESP_LOGE(TAG, "could not create socket: %d", errno);
        return;
    }

    return;
}

void udp_server_begin_packet(char *ip, uint16_t port)
{
    remote_ip = ip;
    remote_port = port;
    recipient = (struct sockaddr_in){
        .sin_addr.s_addr = inet_addr(remote_ip),
        .sin_family = AF_INET,
        .sin_port = htons(remote_port),
    };

    _begin_packet();
}

void udp_server_end_packet()
{
    ESP_LOGI(TAG, "Sending to %s:%d", inet_ntoa(recipient.sin_addr), ntohs(recipient.sin_port));
    int sent = sendto(udp_server, tx_buffer, tx_buffer_len, 0, (struct sockaddr *)&recipient, sizeof(recipient));
    ESP_LOGI(TAG, "Sent %d bytes", sent);
    if (sent < 0)
    {
        ESP_LOGE(TAG, "could not send data: %d", errno);
        return;
    }
    return;
}

void _write_element(char data)
{
    if (tx_buffer_len == 1460)
    {
        udp_server_end_packet();
        tx_buffer_len = 0;
    }
    tx_buffer[tx_buffer_len++] = data;
}

void udp_server_write(const char *buffer, size_t size)
{
    ESP_LOGI(TAG, "Buffer: \"%s\", size: %d", buffer, size);
    for (size_t i = 0; i < size; i++)
        _write_element(buffer[i]);
}
