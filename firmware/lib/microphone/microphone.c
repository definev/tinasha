#include "appconfig.h"
#include "microphone.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"
#include "driver/i2s_common.h"
#include "esp_err.h"

i2s_chan_config_t microphone_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
i2s_std_config_t microphone_rx_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(CONFIG_SAMPLE_RATE),
    .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
    .gpio_cfg = {
        .bclk = I2S_MIC_SERIAL_CLOCK,
        .ws = I2S_MIC_LEFT_RIGHT_CLOCK,
        .din = I2S_MIC_SERIAL_DATA,
    },
};

void microphone_setup(i2s_chan_handle_t *microphone_handle)
{
    ESP_ERROR_CHECK(i2s_new_channel(&microphone_chan_cfg, NULL, microphone_handle));
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(*microphone_handle, &microphone_rx_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(*microphone_handle));
}

void microphone_read(i2s_chan_handle_t microphone_handle, int16_t *mic_buffer)
{
    ESP_ERROR_CHECK(i2s_channel_read(microphone_handle, mic_buffer, MICROPHONE_RECV_BUF_SIZE, NULL, MAX_TIMEOUT));
}