#include "sdkconfig.h"
#include "microphone.h"
#include "speaker.h"
#include "wifi_helper.h"
#include "voice_to_server.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_event.h"

i2s_chan_handle_t microphone_handle;
i2s_chan_handle_t speaker_handle;
voice_to_server_handle_t voice_to_server_handle;

int16_t samples[CONFIG_MICROPHONE_RECV_BUF_SIZE] = {0};

void setup()
{

    wifi_helper_sta_connect();
    microphone_setup(&microphone_handle);
    speaker_setup(&speaker_handle);
    setup_voice_to_server(&voice_to_server_handle);
}

void repeat_microphone(void *arg)
{
    vTaskDelay(1);
    while (1)
    {
        microphone_read(microphone_handle, samples);
        speaker_write(speaker_handle, samples);
        // Send over websocket
        voice_data_t data = voice_to_server_data_from_raw(samples, CONFIG_MICROPHONE_RECV_BUF_SIZE);
        xQueueSend(voice_to_server_handle.queue, &data, portMAX_DELAY);
    }
}

void app_main()
{
    ESP_ERROR_CHECK(nvs_flash_init());
    setup();
    xTaskCreatePinnedToCore(&repeat_microphone, "repeat_microphone", 4096, NULL, 1, NULL, 1);
}