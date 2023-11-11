#include "sdkconfig.h"
#include "app/microphone.h"
#include "app/i2s_common.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"

#define millis() (esp_timer_get_time() / 1000)
#define MAX_ALLOWED_OFFSET 16000
#define MIC_OFFSET_AVERAGING_FRAMES 1
#define VAD_MIC_EXTEND 20000 // ensure there's always another 5s after last VAD detected by server to avoid cutting off while talking

static const char *TAG = "microphone";

int32_t mic_buffer[I2S_BUFFER_SIZE];

void transform_mic_data(int16_t *data)
{
    for (int i = 0; i < I2S_BUFFER_SIZE; i++)
    {
        data[i] = (int16_t)(mic_buffer[i] >> 14);
    }
}

void microphone_setup(microphone_handle_t *handle)
{
    ESP_LOGI(TAG, "Start new microphone:");
    ESP_LOGI(TAG, "- Sample rate: %d", I2S_SAMPLE_RATE);
    ESP_LOGI(TAG, "- Serial clock: %d", I2S_BCK_PIN);
    ESP_LOGI(TAG, "- Word select: %d", I2S_WS_PIN);
    ESP_LOGI(TAG, "- Data line: %d", I2S_SD_IN);
    ESP_LOGI(TAG, "microphone initialized");
    handle->buffer_size = I2S_BUFFER_SIZE;
    handle->timeout = millis() + VAD_MIC_EXTEND;
    ESP_LOGI(TAG, "buffer size: %d + %d", sizeof(handle->buffer), handle->buffer_size);
    ESP_LOGI(TAG, "timeout: %lld", handle->timeout);
    ESP_LOGI(TAG, "microphone setup done");
}

void microphone_read(int16_t *data, size_t *bytes_read)
{
    i2s_read(I2S_PORT, mic_buffer, I2S_BUFFER_SIZE, bytes_read, portMAX_DELAY);
    *bytes_read = *bytes_read / 2;
    transform_mic_data(data);
}