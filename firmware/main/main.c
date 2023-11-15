#include "app/i2s_common.h"
#include "app/wifi_helper.h"
#include "app/voice_to_server.h"
#include "app/vts_protocol/udp.h"
#include "app/microphone.h"
#include "app/speaker.h"
#include "os/udp_server.h"

#include "os/tcp_server.h"
#include "os/command/app_header.h"

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

int64_t extends_seconds(uint8_t seconds)
{
    return millis() + (int64_t)(seconds * 1000);
}

static const char *TAG = "app_main";

static wifi_helper_handle_t wifi_helper_handle;

static microphone_handle_t microphone_handle = {};

volatile bool audio_playing = false;

wav_size_t *wav_data = NULL;
size_t bytes_written;
volatile uint8_t volume = 14; // crude speaker volume control by bitshifting received audio, also set in header. Works well for loudness perception

tcp_server_handle_t tcp_server_handle;

void repeat_microphone_task(void *arg)
{
    ESP_LOGI(TAG, "repeat_microphone_task task started");
    while (1)
    {
        vTaskDelay(1);
        if (audio_playing == true)
        {
            goto skip_microphone;
        }
        if (wifi_helper_handle.connected == false)
        {
            goto skip_microphone;
        }
        if (microphone_handle.timeout < millis())
        {
            goto skip_microphone;
        }

        microphone_read(microphone_handle.buffer, &microphone_handle.bytes_read);
        voice_to_server_udp_callback((const char *)microphone_handle.buffer, microphone_handle.bytes_read);

    skip_microphone:
        microphone_handle.bytes_read = 0;
        continue;
    }
}

size_t tcp_bytes_read;
uint8_t tcp_buff[CONFIG_TCP_BUFFER_SIZE];
size_t buffer_threshold = 8192;
bool finish_receiving = false;

static int64_t _tic = 0;
static uint16_t _timeout = 0;
void _handle_receive_wav_header(uint8_t *header)
{
    audio_playing = true;

    command_header_parse_volume(header, &volume);
    command_header_parse_timeout(header, &_timeout);

    _tic = millis();
    static size_t total_sample_read = 0;
    static bool pass_first_buffer_filled = false;
    static size_t bytes_to_write;

    finish_receiving = false;

    while (tcp_server_is_client_alive(&tcp_server_handle) && !finish_receiving)
    {
        tcp_bytes_read = tcp_server_receive_data(&tcp_server_handle, tcp_buff, CONFIG_TCP_BUFFER_SIZE);
        if (tcp_bytes_read == -1)
        {
            ESP_LOGI(TAG, "error reading tcp bytes: %d %s", errno, strerror(errno));
            if (finish_receiving == false)
            {
                finish_receiving = true;
            }
            break;
        }

        speaker_append_tcp_to_wav(tcp_buff, tcp_bytes_read, wav_data, &total_sample_read, volume);

        if (pass_first_buffer_filled || total_sample_read >= buffer_threshold)
        {
            if (!pass_first_buffer_filled)
            {
                pass_first_buffer_filled = true;
            }

            bytes_to_write = total_sample_read * sizeof(wav_size_t);
            bytes_written = 0;
            speaker_write((char *)wav_data, bytes_to_write, &bytes_written);
            total_sample_read = 0;
        }
    }

    // Hack to fill buffers with silence and block till all real audio is flushed out
    uint32_t silenceBuffer[240];
    memset(silenceBuffer, 0, sizeof(silenceBuffer));
    for (int i = 0; i < 8; i++)
    {
        size_t bytesWritten = 0;
        speaker_write((char *)silenceBuffer, sizeof(silenceBuffer), &bytesWritten);
    }

    audio_playing = false;

    microphone_handle.timeout = extends_seconds(_timeout);

    ESP_LOGI(TAG, "tcp_server_is_client_alive: %d", tcp_server_is_client_alive(&tcp_server_handle));
    ESP_LOGI(TAG, "Done loading audio in buffers in %lld ms", millis() - _tic);
    ESP_LOGI(TAG, "Extended microphone %ds", _timeout);
}

void _handle_adjust_volume_header(uint8_t *header)
{
    command_header_parse_volume(header, &volume);
    ESP_LOGI(TAG, "Set volume to %d", volume);
}

void _handle_microphone_timeout_header(uint8_t *header)
{
    command_header_parse_timeout(header, &_timeout);
    if (_timeout == 0)
    {
        ESP_LOGI(TAG, "stop listening to microphone");
        return;
    }

    microphone_handle.timeout = extends_seconds(_timeout);
    ESP_LOGI(TAG, "Set microphone_timeout to %lld", microphone_handle.timeout);
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
    command_header_log(app_header);

    switch (command_header_get_type(app_header))
    {
    case HEADER_TYPE_RECEIVE_WAV:
        ESP_LOGI(TAG, "Receiving wav");
        _handle_receive_wav_header(app_header);
        break;
    case HEADER_TYPE_ADJUST_VOLUME:
        ESP_LOGI(TAG, "Adjusting volume");
        _handle_adjust_volume_header(app_header);
        break;
    case HEADER_TYPE_MICROPHONE_TIMEOUT:
        ESP_LOGI(TAG, "Setting microphone timeout");
        _handle_microphone_timeout_header(app_header);
        break;
    default:
        ESP_LOGI(TAG, "Unknown header type");
        break;
    }
    tcp_server_diconnect_client(&tcp_server_handle);
}

void tcp_server_task(void *arg)
{
    ESP_LOGI(TAG, "tcp_server_task task started");
    while (1)
    {
        tcp_server_event_handler();
    }
    tcp_server_shutdown(&tcp_server_handle);
}

void app_main()
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

    microphone_setup(&microphone_handle);
    speaker_setup();

    udp_server_begin_packet(CONFIG_TINASHA_MULTICAST_IP, CONFIG_TINASHA_MULTICAST_PORT);
    udp_server_write("tinasha-v1", 10);
    udp_server_end_packet();

    voice_to_server_udp_setup((voice_to_server_udp_config_t){
        .ip_addr = CONFIG_VTS_UDP_SERVER_IP,
        .port = CONFIG_VTS_UDP_SERVER_PORT,
    });
    // voice_to_server_ws_setup((vts_ws_config_t){
    //     .uri = CONFIG_WS_URI,
    //     .buffer_size = VTS_WS_BUFFER_SIZE,
    // });

    tcp_server_setup(&tcp_server_handle, CONFIG_TINASHA_TCP_SERVER_PORT);

    /// Schedule task
    {
        xTaskCreatePinnedToCore(&repeat_microphone_task, "repeat_microphone", 4096, NULL, configMAX_PRIORITIES, NULL, 0);
        xTaskCreatePinnedToCore(&tcp_server_task, "tcp_server", 4096, NULL, configMAX_PRIORITIES, NULL, 1);
    }
}