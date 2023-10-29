#include "sdkconfig.h"
#include "microphone.h"
#include "i2s/i2s.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"
#include "driver/i2s_common.h"
#include "esp_err.h"

void microphone_setup(i2s_chan_handle_t *microphone_handle)
{
    i2s_chan_config_t microphone_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&microphone_chan_cfg, NULL, microphone_handle));

    i2s_std_config_t microphone_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_SERIAL_CLOCK,
            .ws = I2S_WORD_SELECT,
            .din = I2S_MIC_SERIAL_DATA,
            .dout = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(*microphone_handle, &microphone_cfg));

    ESP_ERROR_CHECK(i2s_channel_enable(*microphone_handle));
}

void microphone_read(i2s_chan_handle_t microphone_handle, char *mic_buffer, size_t *bytes_read)
{
    ESP_ERROR_CHECK(i2s_channel_read(microphone_handle, mic_buffer, I2S_RX_SAMPLE_SIZE, bytes_read, CONFIG_MAX_TIMEOUT));
}