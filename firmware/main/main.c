#include "app/i2s_common.h"
#include "app/wifi_helper.h"
#include "app/voice_to_server.h"
#include "app/vts_protocol/ws.h"
#include "app/microphone.h"
#include "app/speaker.h"
#include "app/tcp_server.h"

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_event.h"
#include "sys/socket.h"
#include "driver/i2s.h"

#define millis() (esp_timer_get_time() / 1000)

static const char *TAG = "app_main";

wifi_helper_handle_t wifi_helper_handle;

typedef enum
{
    /*
    header[0] = 0x00 header type
    header[1:2] = mic timeout after playing (ms)
    header[3] = value
    header[4] = none
    header[5] = none
    */
    HEADER_TYPE_RECEIVE_WAV = 0xAA,
    /*
    header[0] = 0x01 header type
    header[1] = increase/decrease volume
    header[2] = value
    header[3] = none
    header[4] = none
    header[5] = none
    */
    HEADER_TYPE_ADJUST_VOLUME = 0xAB,
} header_type_t;
void log_header_buffer(uint8_t *header)
{
    ESP_LOGI(TAG, "(%d, %d, %d, %d, %d, %d)", header[0], header[1], header[2], header[3], header[4], header[5]);
}

// i2s_chan_handle_t speaker_handle;
voice_to_server_handle_t voice_to_server_handle;

int16_t mic_buff[I2S_BUFFER_SIZE];
size_t bytes_read;

int32_t *wav_data = NULL;
size_t bytes_written;
uint8_t volume = 16;

tcp_server_handle_t tcp_server_handle;

void repeat_microphone(void *arg)
{
    ESP_LOGI(TAG, "repeat_microphone task started");
    while (1)
    {
        bytes_read = 0;
        microphone_read(mic_buff, &bytes_read);
        {
            voice_to_server_ws_callback((char *)mic_buff, bytes_read);
        }
    }
}
size_t tcp_bytes_read;
uint8_t tcp_buff[CONFIG_TCP_BUFFER_SIZE];
size_t buffer_threshold = 8192;
bool finish_receiving = false;

void _handle_receive_wav_header(uint8_t *header)
{

    static size_t total_sample_read = 0;
    static bool pass_first_buffer_filled = false;
    static size_t bytes_to_write;
    static uint16_t sample;

    finish_receiving = false;

    vTaskDelay(pdMS_TO_TICKS(200));

    while (tcp_server_is_client_alive(&tcp_server_handle) && !finish_receiving)
    {
        tcp_bytes_read = tcp_server_receive_data(&tcp_server_handle, tcp_buff, CONFIG_TCP_BUFFER_SIZE);
        if (tcp_bytes_read == -1)
        {
            ESP_LOGI(TAG, "error reading tcp bytes: %d %s", errno, strerror(errno));
            if (finish_receiving == false)
            {
                finish_receiving = true;
                break;
            }
            continue;
        }

        {
            for (size_t i = 0; i < tcp_bytes_read; i += 2)
            {
                sample = tcp_buff[i + 1] << 8 | tcp_buff[i];
                wav_data[total_sample_read++] = (int32_t)sample << volume;
            }
        }

        if (pass_first_buffer_filled || total_sample_read >= buffer_threshold)
        {
            // ESP_LOGI(TAG, "total_sample_read: %d samples", total_sample_read);
            if (!pass_first_buffer_filled)
            {
                // ESP_LOGI(TAG, "pass_first_buffer_filled: %d samples", total_sample_read);
                pass_first_buffer_filled = true;
            }

            bytes_to_write = total_sample_read * sizeof(int32_t);
            bytes_written = 0;
            voice_to_server_ws_callback((char *)wav_data, bytes_to_write);
            // speaker_write((char *)wav_data, bytes_to_write, &bytes_written);
            // ESP_LOGI(TAG, "speaker_write: %d samples", bytes_written);
            total_sample_read = 0;
        }
    }
    ESP_LOGI(TAG, "tcp_server_is_client_alive: %d", tcp_server_is_client_alive(&tcp_server_handle));
}

void tcp_server_event_handler()
{
    int client_sock = tcp_server_find_client(&tcp_server_handle);
    if (client_sock < 0)
    {
        return;
    }

    struct sockaddr_in *addr_in = (struct sockaddr_in *)&tcp_server_handle.remote_addr;
    ESP_LOGI(
        TAG,
        "Accepted connection from %s%d\n",
        inet_ntoa(addr_in->sin_addr),
        ntohs(addr_in->sin_port));

    static uint8_t app_header[6];
    tcp_server_receive_header(&tcp_server_handle, app_header);
    log_header_buffer(app_header);

    switch (app_header[0])
    {
    case HEADER_TYPE_RECEIVE_WAV:
        ESP_LOGI(TAG, "Receiving wav");
        _handle_receive_wav_header(app_header);
        break;
    case HEADER_TYPE_ADJUST_VOLUME:
        ESP_LOGI(TAG, "Adjusting volume");
        // _handle_adjust_volume_header(app_header);
        break;
    }
    tcp_server_diconnect_client(&tcp_server_handle);
}

void tcp_server_handler(void *arg)
{
    ESP_LOGI(TAG, "tcp_server_handler task started");
    while (1)
    {
        tcp_server_event_handler();
    }
    tcp_server_shutdown(&tcp_server_handle);
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

        wav_data = speaker_init_buffer();
    }

    wifi_helper_connect(&wifi_helper_handle);

    app_i2s_warmup();

    microphone_setup();
    speaker_setup();

    // voice_to_server_udp_setup((voice_to_server_udp_config_t){
    //     .ip_addr = app_state.wifi_status->ip_addr,
    //     .target_port = CONFIG_VTS_UDP_SERVER_PORT,
    // });
    voice_to_server_ws_setup(
        (vts_ws_config_t){
            .uri = CONFIG_WS_URI,
            .buffer_size = VTS_WS_BUFFER_SIZE,
        });

    tcp_server_handle = tcp_server_setup(CONFIG_TINASHA_TCP_SERVER_PORT);

    /// Schedule task
    {
        //     xTaskCreatePinnedToCore(&repeat_microphone, "repeat_microphone", 4096, NULL, 1, NULL, 0);
        xTaskCreate(&tcp_server_handler, "tcp_server", 4096, NULL, configMAX_PRIORITIES, NULL);
    }
}

void app_main()
{
    setup();
}