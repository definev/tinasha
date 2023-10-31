#include "sdkconfig.h"
#include "i2s/i2s.h"
#include "speaker.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"
#include "driver/i2s_common.h"
#include "esp_err.h"

void speaker_setup(i2s_chan_handle_t *speaker_handle)
{
    i2s_chan_config_t speaker_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&speaker_chan_cfg, speaker_handle, NULL));

    i2s_std_config_t speaker_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_SERIAL_CLOCK,
            .ws = I2S_WORD_SELECT,
            .din = I2S_GPIO_UNUSED,
            .dout = I2S_SPEAKER_SERIAL_DATA,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = true,
                .ws_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(*speaker_handle, &speaker_std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(*speaker_handle));
}

void speaker_write(i2s_chan_handle_t speaker_handle, char *data, size_t *bytes_written)
{
    ESP_ERROR_CHECK(i2s_channel_write(speaker_handle, data, I2S_BUFFER_SIZE, bytes_written, CONFIG_MAX_TIMEOUT));
}
