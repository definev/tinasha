#include "sdkconfig.h"
#include "i2s/i2s.h"
#include "wifi_helper.h"
#include "voice_to_server.h"
#include "microphone.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_event.h"
#include "driver/i2s_std.h"
#include "driver/i2s_types.h"

#define millis() (esp_timer_get_time() / 1000)

const char *MAIN_TAG = "main";

i2s_chan_handle_t microphone_handle;
voice_to_server_handle_t voice_to_server_handle;

int16_t mic_buff[I2S_BUFFER_SIZE];
size_t bytes_read;

size_t bytes_written;

void setup()
{
    wifi_helper_connect();
    microphone_init(&microphone_handle);
    voice_to_server_init(&voice_to_server_handle);
}

void repeat_microphone(void *arg)
{
    ESP_LOGI(MAIN_TAG, "repeat_microphone task started");

    while (1)
    {
        bytes_read = 0;
        microphone_read(microphone_handle, mic_buff, I2S_BUFFER_SIZE, &bytes_read);
        voice_to_server_send(voice_to_server_handle, (const char *)mic_buff, bytes_read);
        vTaskDelay(1);
    }
}

void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    setup();

    // xTaskCreatePinnedToCore(&voice_to_server_task, "voice_to_server_task", 4096, handle, 1, NULL, 1);
    xTaskCreatePinnedToCore(&repeat_microphone, "repeat_microphone", 4096, NULL, 1, NULL, 0);
}