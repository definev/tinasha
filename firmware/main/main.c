#include "app/i2s.h"
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
    HEADER_TYPE_RECEIVE_WAV = 0x00,
    /*
    header[0] = 0x01 header type
    header[1] = increase/decrease volume
    header[2] = value
    header[3] = none
    header[4] = none
    header[5] = none
    */
    HEADER_TYPE_ADJUST_VOLUME = 0x01,
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
uint8_t volume = 10;

tcp_server_handle_t tcp_server_handle;

void repeat_microphone(void *arg)
{
    ESP_LOGI(TAG, "repeat_microphone task started");
    while (1)
    {
        bytes_read = 0;
        microphone_read(mic_buff, sizeof(mic_buff), &bytes_read);
        {
            voice_to_server_ws_callback((char *)mic_buff, bytes_read);
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

    wifi_helper_connect(&wifi_helper_handle);
    microphone_setup();
    // speaker_setup(&speaker_handle, wav_data);
    // voice_to_server_udp_setup((voice_to_server_udp_config_t){
    //     .ip_addr = app_state.wifi_status->ip_addr,
    //     .port = CONFIG_VTS_UDP_SERVER_PORT,
    // });
    voice_to_server_ws_setup(
        (vts_ws_config_t){
            .uri = CONFIG_WS_URI,
            .buffer_size = VTS_WS_BUFFER_SIZE,
        });
    tcp_server_handle = tcp_server_setup(
        (tcp_server_config_t){
            .port = CONFIG_TINASHA_TCP_SERVER_PORT,
        },
        wifi_helper_handle.ip_addr);

    // Schedule task
    {
        xTaskCreatePinnedToCore(&repeat_microphone, "repeat_microphone", 4096, NULL, 1, NULL, 0);
    }
}

size_t available_bytes;
uint8_t tcp_buff[CONFIG_TCP_BUFFER_SIZE];
size_t buffer_threshold = 8192;

void _handle_receive_wav_header(uint8_t header)
{
    static size_t total_sample_read = 0;
    static size_t bytes_to_read, bytes_read, bytes_to_write, bytes_written;
    static bool pass_first_buffer_filled = false;
    static uint16_t sample;

    while (tcp_server_is_client_alive(&tcp_server_handle))
    {
        available_bytes = tcp_server_ready_to_read(&tcp_server_handle);

        if (available_bytes >= 2)
        {
            bytes_to_read = (available_bytes / 2) * 2; // ensure whole samples only
            if (bytes_to_read > CONFIG_TCP_BUFFER_SIZE)
            {
                bytes_to_read = CONFIG_TCP_BUFFER_SIZE;
            }

            bytes_read = tcp_server_receive_data(&tcp_server_handle, tcp_buff, bytes_to_read);
            for (size_t i = 0; i < bytes_read; i += 2)
            {
                sample = tcp_buff[i + 1] << 8 | tcp_buff[i];
                wav_data[total_sample_read++] = sample;
            }

            if (pass_first_buffer_filled || total_sample_read >= buffer_threshold)
            {
                if (!pass_first_buffer_filled)
                {
                    ESP_LOGI(TAG, "pass_first_buffer_filled: %d samples", total_sample_read);
                    pass_first_buffer_filled = true;
                }

                bytes_to_write = total_sample_read * sizeof(int32_t);
                bytes_written = 0;

                speaker_write(wav_data, &bytes_written);
                total_sample_read = 0;
            }
        }
        else
        {
            vTaskDelay(2);
        }
    }
}

void tcp_server_event_handler()
{
    tcp_server_find_client(&tcp_server_handle);
    if (tcp_server_handle.client_sock_fd < 0)
    {
        ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
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

    switch (app_header[0])
    {
    case HEADER_TYPE_RECEIVE_WAV:
        _handle_receive_wav_header(app_header);
        break;
    case HEADER_TYPE_ADJUST_VOLUME:
        _handle_adjust_volume_header(app_header);
    }
}

void app_main()
{
    setup();
    while (1)
    {
        tcp_server_event_handler();
    }
    
}