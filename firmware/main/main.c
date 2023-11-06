#include "app/i2s.h"
#include "app/wifi_helper.h"
#include "app/voice_to_server.h"
#include "app/vts_protocol/udp.h"
#include "app/microphone.h"
#include "app/speaker.h"

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_event.h"
#include "sys/socket.h"
#include "driver/i2s_std.h"
#include "driver/i2s_types.h"

#define millis() (esp_timer_get_time() / 1000)
#define CREATE_TCP_SERVER() socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

static const char *TAG = "app_main";

typedef enum
{
    TCP_SERVER_STATUS_DISCONNECTED,
    TCP_SERVER_STATUS_CONNECTED,
} tcp_server_status_t;

typedef struct
{
    volatile bool on_playing;
    tcp_server_status_t tcp_server_status;
    wifi_helper_status_t *wifi_status;
} app_state_t;

typedef enum
{
    /*
    header[0] = 0x00 header type
    header[1:2] = mic timeout after playing (ms)
    header[3] = value
    header[4] = none
    header[5] = none
    */
    HEADER_TYPE_RECEIVE_WAV,
    /*
    header[0] = 0x01 header type
    header[1] = increase/decrease volume
    header[2] = value
    header[3] = none
    header[4] = none
    header[5] = none
    */
    HEADER_TYPE_ADJUST_VOLUME,
} header_type_t;
void log_header_buffer(uint8_t *header)
{
    ESP_LOGI(TAG, "(%d, %d, %d, %d, %d, %d)", header[0], header[1], header[2], header[3], header[4], header[5]);
}

app_state_t app_state = {
    .on_playing = false,
    .wifi_status = NULL,
};

i2s_chan_handle_t microphone_handle;
i2s_chan_handle_t speaker_handle;
voice_to_server_handle_t voice_to_server_handle;

int16_t mic_buff[I2S_BUFFER_SIZE];
size_t bytes_read;

int32_t *wav_data = NULL;
size_t bytes_written;
uint8_t volume = 10;

void repeat_microphone(void *arg)
{
    ESP_LOGI(TAG, "repeat_microphone task started");
    while (1)
    {
        bytes_read = 0;
        microphone_read(microphone_handle, mic_buff, sizeof(mic_buff), &bytes_read);
        {
            voice_to_server_udp_callback(mic_buff, bytes_read);
        }
    }
}

void setup()
{
    // boilerplate
    {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
    }

    wifi_helper_connect(app_state.wifi_status);
    microphone_setup(&microphone_handle);
    speaker_setup(&speaker_handle, wav_data);
    voice_to_server_udp_setup((voice_to_server_udp_config_t){
        .ip_addr = app_state.wifi_status->ip_addr,
        .port = CONFIG_VTS_UDP_SERVER_PORT,
    });

    {
        xTaskCreatePinnedToCore(&repeat_microphone, "repeat_microphone", 4096, NULL, 1, NULL, 0);
    }
}

struct sockaddr_in dest_addr = {
    .sin_addr.s_addr = htonl(INADDR_ANY),
    .sin_family = AF_INET,
    .sin_port = htons(CONFIG_TINASHA_TCP_SERVER_PORT),
};
static uint8_t header[6];
static int tcp_server_id = -1;
static struct sockaddr_in remote_addr;
static unsigned int socklen = sizeof(remote_addr);

void loop()
{
    // Start TCP server
    if (app_state.tcp_server_status == TCP_SERVER_STATUS_DISCONNECTED)
    {
        tcp_server_id = CREATE_TCP_SERVER();
        if (tcp_server_id < 0)
        {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            goto handle_tcp_failure;
        }
        if (bind(tcp_server_id, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) != 0)
        {
            ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
            goto handle_tcp_failure;
        }

        if (listen(tcp_server_id, 2) < 0)
        {
            ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
            goto handle_tcp_failure;
        }

        // handle_tcp_success:
        app_state.tcp_server_status = TCP_SERVER_STATUS_CONNECTED;
    handle_tcp_failure:
        close(tcp_server_id);
        app_state.tcp_server_status = TCP_SERVER_STATUS_DISCONNECTED;
        return;
    }

    while (1)
    {
        int tcp_client_id = accept(tcp_server_id, (struct sockaddr *)&remote_addr, socklen);

        //----- A CLIENT HAS CONNECTED -----
        ESP_LOGI(TAG, "New client connection");
        int received_bytes = recv(tcp_client_id, header, 6, MSG_PEEK);
        if (received_bytes == 0)
        {
            ESP_LOGI(TAG, "Failed to read header");
            vTaskDelay(pdMS_TO_TICKS(5000));
            return;
        }
        else if (received_bytes < 0)
        {
            ESP_LOGI(TAG, "Failed to read header: errno %d", errno);
            vTaskDelay(pdMS_TO_TICKS(5000));
            return;
        }
        else
        {
            log_header_buffer(header);
            switch (header[0])
            {
            case HEADER_TYPE_RECEIVE_WAV:
                uint16_t timeout = header[1] << 8 | header[2];
                ESP_LOGI(TAG, "Received audio with mic timeout of %d seconds and volume of %d\n", timeout, volume);
                app_state.on_playing = true;

                bool initialBufferFilled = false; // get a nice reservoir loaded into wavData to try avoid jitter
                uint32_t tic = millis();
                size_t totalSamplesRead = 0;

                size_t bytes_available, bytes_to_read, read_bytes, bytes_written, bytes_to_write;
                int16_t sample16;

                while (1)
                {
                    uint8_t tcp_buffer[CONFIG_TCP_BUFFER_SIZE];
                    read_bytes = recv(tcp_client_id, tcp_buffer, CONFIG_TCP_BUFFER_SIZE, MSG_PEEK);
                }

                break;
            case HEADER_TYPE_ADJUST_VOLUME:
                break;
            }
        }
    }
}

void app_main()
{
    setup();
    while (1)
    {
        loop();
    }
}