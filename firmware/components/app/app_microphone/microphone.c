#include "sdkconfig.h"
#include "app/microphone.h"
#include "app/i2s_common.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"
#include "esp_err.h"
#include "esp_log.h"

static const char *TAG = "microphone";

int32_t mic_buffer[I2S_BUFFER_SIZE];

void transform_mic_data(int16_t *data)
{
    for (int i = 0; i < I2S_BUFFER_SIZE; i++)
    {
        data[i] = (int16_t)(mic_buffer[i] >> 14);
    }
}
void microphone_setup(void)
{
    const i2s_config_t i2s_config = {
        .mode = (I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BIT_PER_SAMPLE,
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
        .data_out_num = -1,
        .data_in_num = I2S_MICROPHONE_SERIAL_DATA,
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
    i2s_start(I2S_PORT);

    ESP_LOGI(TAG, "Start new microphone:\n");
    ESP_LOGI(TAG, "- Sample rate: %d", I2S_SAMPLE_RATE);
    ESP_LOGI(TAG, "- Serial clock: %d", I2S_SERIAL_CLOCK);
    ESP_LOGI(TAG, "- Word select: %d", I2S_WORD_SELECT);
    ESP_LOGI(TAG, "- Data line: %d", I2S_MICROPHONE_SERIAL_DATA);
    ESP_LOGI(TAG, "microphone initialized");
}

void microphone_read(int16_t *data, size_t *bytes_read)
{
    i2s_read(I2S_PORT, mic_buffer, I2S_BUFFER_SIZE, bytes_read, portMAX_DELAY);
    transform_mic_data(data);
}