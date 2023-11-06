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
struct sockaddr_in udp_server_addr;

int curr_sock = -1;

int voice_to_server_udp_init()
{
    if (curr_sock < 0)
    {
        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            return -1;
        }
        if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
        {
            ESP_LOGE(TAG, "error in setsockopt SO_RCVTIMEO (%d)", errno);
            return -1;
        }
        int keep_alive = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive)) < 0)
        {
            ESP_LOGE(TAG, "error in setsockopt SO_KEEPALIVE (%d)", errno);
            return -1;
        }

        ESP_LOGI(TAG, "Socket created, sending to %s:%d", CONFIG_VTS_UDP_SERVER_IP, CONFIG_VTS_UDP_SERVER_PORT);
        return sock;
    }
    else
    {
        return curr_sock;
    }

    return -1;
}

void voice_to_server_udp_setup(voice_to_server_udp_config_t config)
{
    udp_server_addr
        .sin_addr
        .s_addr = config.ip_addr;
    udp_server_addr
        .sin_port = htons(config.port);
    udp_server_addr
        .sin_family = AF_INET;

    curr_sock = voice_to_server_udp_init();
    int retry_count = 0;
    while (retry_count < MAX_RETRY_COUNT && curr_sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        ESP_LOGI(TAG, "Retrying in 5 seconds...");
        vTaskDelay(pdMS_TO_TICKS(5000));
        curr_sock = voice_to_server_udp_init();
        retry_count++;
    }
}

void voice_to_server_udp_callback(void *data, size_t size)
{
    curr_sock = voice_to_server_udp_init();
    while (1)
    {
        if (sendto(curr_sock, data, size, 0, (struct sockaddr *)&udp_server_addr, sizeof(udp_server_addr)) < 0)
        {
            curr_sock = -1;
            ESP_LOGE(TAG, "Unable to send data: errno %d", errno);
            curr_sock = voice_to_server_udp_init();
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