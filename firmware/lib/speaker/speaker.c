#include "speaker.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"
#include "driver/i2s_common.h"
#include "esp_err.h"

i2s_chan_config_t speaker_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
i2s_std_config_t speaker_std_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
    .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
    .gpio_cfg = {
        .mclk = I2S_GPIO_UNUSED,
        .bclk = I2S_SPEAKER_SERIAL_CLOCK,
        .ws = I2S_SPEAKER_LEFT_RIGHT_CLOCK,
        .dout = I2S_SPEAKER_SERIAL_DATA,
        .invert_flags = {
            .mclk_inv = false,
            .bclk_inv = false,
            .ws_inv = false,
        },
    },
};

void setup_speaker(i2s_chan_handle_t *speaker_handle)
{
    /* Allocate a new TX channel and get the handle of this channel */
    ESP_ERROR_CHECK(i2s_new_channel(&speaker_chan_cfg, speaker_handle, NULL));
    /* Initialize the I2S channel in standard mode */
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(*speaker_handle, &speaker_std_cfg));
    /* Enable the I2S channel */
    ESP_ERROR_CHECK(i2s_channel_enable(*speaker_handle));
}

void write_speaker(i2s_chan_handle_t *speaker_handle, int16_t *data, size_t len)
{
    /* Write data to the I2S channel */
    ESP_ERROR_CHECK(i2s_channel_write(speaker_handle, NULL, len, (size_t *)data, portMAX_DELAY));
}
