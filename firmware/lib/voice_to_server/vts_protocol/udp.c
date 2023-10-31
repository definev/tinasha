#include "udp.h"
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
        ESP_LOGI(TAG, "Socket already created");
        return curr_sock;
    }

    int sock = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0)
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        goto exit;
    }
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
    {
        ESP_LOGE(TAG, "error in setsockopt SO_RCVTIMEO (%d)", errno);
        goto exit;
    }

    int keep_alive = 1;
    if (lwip_setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, sizeof(keep_alive)) < 0)
    {
        ESP_LOGE(TAG, "error in setsockopt SO_KEEPALIVE (%d)", errno);
        goto exit;
    }

    ESP_LOGI(TAG, "Socket created, sending to %s:%d", CONFIG_VTS_UDP_SERVER_IP, CONFIG_VTS_UDP_SERVER_PORT);

exit:
    return -1;
}

void voice_to_server_udp_setup()
{
    udp_server_addr
        .sin_addr
        .s_addr = inet_addr(CONFIG_VTS_UDP_SERVER_IP);
    udp_server_addr
        .sin_port = htons(CONFIG_VTS_UDP_SERVER_PORT);
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

void voice_to_server_udp_callback(char *data, size_t size)
{
    curr_sock = voice_to_server_udp_init();
    int retry_count = 0;
    while (1)
    {
        if (lwip_sendto(curr_sock, data, size, 0, (struct sockaddr *)&udp_server_addr, sizeof(udp_server_addr)) < 0)
        {
            ESP_LOGE(TAG, "Unable to send data: errno %d", errno);
            curr_sock = voice_to_server_udp_init();
        }
        else
        {
            ESP_LOGI(TAG, "Data sent");
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