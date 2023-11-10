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

wav_size_t *speaker_init_buffer()
{
    wav_size_t *wav_data;
    /* Initialize WAV form */
    size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    size_t total_psram = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);

    ESP_LOGI(TAG, "PSRAM free: %d", free_psram);
    ESP_LOGI(TAG, "PSRAM total: %d", total_psram);
    ESP_LOGI(TAG, "PSRAM used: %d", total_psram - free_psram);

    wav_data = (wav_size_t *)malloc((2 * 1024 * 1024) / sizeof(wav_size_t));
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

static uint16_t sample;
void speaker_append_tcp_to_wav(
    uint8_t *tcp_data,
    size_t tcp_data_size,
    wav_size_t *wav_data,
    size_t *wav_data_size,
    uint8_t volume)
{

    for (size_t i = 0; i < tcp_data_size; i += 2)
    {
        sample = tcp_data[i + 1] << 8 | tcp_data[i];
        wav_data[*wav_data_size] = (wav_size_t)sample << volume;
        (*wav_data_size)++;
    }
}

void speaker_setup(void)
{
    ESP_LOGI(TAG, "Start speaker:");
    ESP_LOGI(TAG, "- Sample rate: %d", I2S_SAMPLE_RATE);
    ESP_LOGI(TAG, "- Serial clock: %d", I2S_BCK_PIN);
    ESP_LOGI(TAG, "- Word select: %d", I2S_WS_PIN);
    ESP_LOGI(TAG, "- Data line: %d", I2S_SD_OUT);
    ESP_LOGI(TAG, "Speaker initialized");
}

void speaker_write(const char *data, size_t size, size_t *bytes_written)
{
    ESP_ERROR_CHECK(i2s_write(I2S_PORT, data, size, bytes_written, portMAX_DELAY));
}