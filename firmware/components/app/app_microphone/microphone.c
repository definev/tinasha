#include "sdkconfig.h"
#include "app/microphone.h"
#include "app/i2s.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"
#include "driver/i2s_common.h"
#include "esp_err.h"
#include "esp_log.h"

static const char* TAG = "microphone";

void transform_mic_data(int16_t *data, size_t size)
{
    for (int i = 0; i < size; i++)
    {
        data[i] = (int16_t)data[i] << 3;
    }
}

void microphone_setup(i2s_chan_handle_t *handle)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    i2s_std_config_t i2s_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(MICROPHONE_DATA_WIDTH, MICROPHONE_SLOT_MODE),
        .gpio_cfg = {
            .bclk = I2S_SERIAL_CLOCK,
            .ws = I2S_WORD_SELECT,
            .din = I2S_MIC_SERIAL_DATA,
        },
    };
    ESP_LOGI(TAG, "Start new microphone:\n");
    ESP_LOGI(TAG, "- Data width: %d", MICROPHONE_DATA_WIDTH);
    ESP_LOGI(TAG, "- Slot mode: %d", MICROPHONE_SLOT_MODE);
    ESP_LOGI(TAG, "- Sample rate: %d", I2S_SAMPLE_RATE);
    ESP_LOGI(TAG, "- Serial clock: %d", I2S_SERIAL_CLOCK);
    ESP_LOGI(TAG, "- Word select: %d", I2S_WORD_SELECT);
    ESP_LOGI(TAG, "- Data line: %d", I2S_MIC_SERIAL_DATA);

    i2s_new_channel(&chan_cfg, NULL, handle);
    i2s_channel_init_std_mode(*handle, &i2s_std_cfg);
    i2s_channel_enable(*handle);
    ESP_LOGI(TAG, "microphone initialized");
}

void microphone_read(i2s_chan_handle_t handler, void *mic_buffer, size_t buffer_size, size_t *bytes_read)
{
    ESP_ERROR_CHECK(i2s_channel_read(handler, mic_buffer, buffer_size, bytes_read, portMAX_DELAY));
}