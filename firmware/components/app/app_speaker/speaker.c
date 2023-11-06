#include <stdlib.h>

#include "sdkconfig.h"
#include "app/i2s.h"
#include "app/speaker.h"

#include "esp_log.h"

// #include "freertos/FreeRTOS.h"
// #include "driver/i2s.h"
// #include "esp_err.h"
// #include "esp_log.h"
// #include "esp_heap_caps.h"

// how many samples to load from TCP before starting playing (avoid jitter due to running out of data w/ bad wifi)
#ifdef CONFIG_SPIRAM_SIZE
int bufferThreshold = 8192;
#else
int bufferThreshold = 512;
#endif

static const char *TAG = "speaker";

void speaker_setup(void)
{
    /* Initialize WAV form */
    // size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    // size_t total_psram = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);

    // ESP_LOGI(TAG, "PSRAM free: %d", free_psram);
    // ESP_LOGI(TAG, "PSRAM total: %d", total_psram);
    // ESP_LOGI(TAG, "PSRAM used: %d", total_psram - free_psram);

    // wav_data = (int32_t *)malloc((2 * 1024 * 1024) / sizeof(int32_t));
    // if (wav_data == NULL)
    // {
    //     ESP_LOGE(TAG, "Memory allocation failed!");
    //     while (1)
    //         ;
    // }
    // else
    // {
    //     ESP_LOGI(TAG, "Memory allocation successful!");
    // }
    // free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
    // ESP_LOGI(TAG, "PSRAM used: %d", total_psram - free_psram);

    /* Initialize speaker */

    const i2s_config_t i2s_config = {
        .mode = (I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_SPEAKER_BIT_PER_SAMPLE,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = 0,
        .dma_buf_count = I2S_DMA_DESC_NUM,
        .dma_buf_len = I2S_DMA_FRAME_NUM,
        .use_apll = false,
    };

    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SERIAL_CLOCK,
        .ws_io_num = I2S_WORD_SELECT,
        .data_out_num = I2S_SPEAKER_SERIAL_DATA,
        .data_in_num = -1,
    };

    i2s_driver_install(I2S_SPEAKER_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_SPEAKER_PORT, &pin_config);
    i2s_start(I2S_SPEAKER_PORT);

    ESP_LOGI(TAG, "Start new speaker:\n");
    ESP_LOGI(TAG, "- Sample rate: %d", I2S_SAMPLE_RATE);
    ESP_LOGI(TAG, "- Serial clock: %d", I2S_SERIAL_CLOCK);
    ESP_LOGI(TAG, "- Word select: %d", I2S_WORD_SELECT);
    ESP_LOGI(TAG, "- Data line: %d", I2S_SPEAKER_SERIAL_DATA);
    ESP_LOGI(TAG, "speaker initialized");
}

void speaker_write(char *data, size_t *bytes_written)
{
    ESP_ERROR_CHECK(i2s_write(I2S_SPEAKER_PORT, data, I2S_BUFFER_SIZE, bytes_written, portMAX_DELAY));
}

// void speaker_write(char *data, size_t *bytes_written)
// {
//     ESP_ERROR_CHECK(i2s_channel_write(speaker_handle, data, I2S_BUFFER_SIZE, bytes_written, CONFIG_MAX_TIMEOUT));
// }
