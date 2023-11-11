#include "app/vts_protocol/udp.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
#include "esp_event.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"

static const char *TAG = "voice_to_server_udp";

struct timeval timeout = {.tv_sec = 10, .tv_usec = 0};
struct sockaddr_in dest_addr;

int curr_sock = -1;

int voice_to_server_udp_init()
{
    if (curr_sock > 0)
        return curr_sock;
    int retry_count = 0;
    while (retry_count < 5)
    {

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            retry_count++;
            continue;
        }
        else
        {
            ESP_LOGI(TAG, "Created UDP socket %d", sock);
            return sock;
        }
    }
    return -1;
}

void voice_to_server_udp_setup(voice_to_server_udp_config_t config)
{
    dest_addr
        .sin_addr
        .s_addr = inet_addr(config.ip_addr);
    dest_addr
        .sin_family = AF_INET;
    dest_addr
        .sin_port = htons(config.port);

    ESP_LOGI(TAG, "Setting up UDP socket");
    ESP_LOGI(TAG, "Sending to %s:%d", inet_ntoa(dest_addr.sin_addr), ntohs(dest_addr.sin_port));

    curr_sock = voice_to_server_udp_init();
}

void voice_to_server_udp_callback(const char *data, size_t size)
{
    curr_sock = voice_to_server_udp_init();
    int retry_count = 0;
    while (retry_count < 5)
    {
        int err = sendto(curr_sock, data, size, 0, (const struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err < 0)
        {
            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
            retry_count++;
            continue;
        }
        else
        {
            break;
        }
    }
}

void voice_to_server_udp_close()
{
    if (curr_sock >= 0)
    {
        lwip_close(curr_sock);
        curr_sock = -1;
    }
}