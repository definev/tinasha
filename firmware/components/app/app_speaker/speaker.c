#include <stdlib.h>

#include "sdkconfig.h"
#include "app/i2s_common.h"
#include "app/speaker.h"

#include "esp_log.h"

// #include "freertos/FreeRTOS.h"
// #include "driver/i2s.h"
// #include "esp_err.h"
// #include "esp_log.h"
// #include "esp_heap_caps.h"

// how many samples to load from TCP before starting playing (avoid jitter due to running out of data w/ bad wifi)
#ifdef CONFIG_ESP32S3_SPIRAM_SUPPORT
int bufferThreshold = 8192;
#else
int bufferThreshold = 512;
#endif

static const char *TAG = "speaker";

int32_t *speaker_init_buffer()
{
    int32_t *wav_data;
    /* Initialize WAV form */
    size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    size_t total_psram = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);

    ESP_LOGI(TAG, "PSRAM free: %d", free_psram);
    ESP_LOGI(TAG, "PSRAM total: %d", total_psram);
    ESP_LOGI(TAG, "PSRAM used: %d", total_psram - free_psram);

    wav_data = (int32_t *)malloc((2 * 1024 * 1024) / sizeof(int32_t));
    if (wav_data == NULL)
    {
        ESP_LOGE(TAG, "Memory allocation failed!");
        while (1)
            ;
    }
    else
    {
        ESP_LOGI(TAG, "Memory allocation successful!");
    }
    free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    ESP_LOGI(TAG, "PSRAM used: %d", total_psram - free_psram);
    return wav_data;
}

void speaker_setup(void)
{
    ESP_LOGI(TAG, "Start speaker:");
    ESP_LOGI(TAG, "- Sample rate: %d", I2S_SAMPLE_RATE);
    ESP_LOGI(TAG, "- Serial clock: %d", I2S_BCK_PIN);
    ESP_LOGI(TAG, "- Word select: %d", I2S_WORD_SELECT);
    ESP_LOGI(TAG, "- Data line: %d", I2S_SPEAKER_SERIAL_DATA);
    ESP_LOGI(TAG, "Speaker initialized");
}

void speaker_write(const void *data, size_t size, size_t *bytes_written)
{
    ESP_ERROR_CHECK(i2s_write(I2S_PORT, data, size, bytes_written, portMAX_DELAY));
}