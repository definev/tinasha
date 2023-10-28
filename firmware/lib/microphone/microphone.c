#include "sdkconfig.h"
#include "microphone.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"
#include "driver/i2s_common.h"
#include "esp_err.h"

i2s_chan_config_t microphone_chan_cfg = {
    .id = I2S_NUM_0,
    .role = I2S_ROLE_MASTER,
    .dma_desc_num = 10,
    .dma_frame_num = 1024,
    .auto_clear = false,
};

i2s_std_config_t microphone_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(CONFIG_SAMPLE_RATE),
    .slot_cfg = I2S_STD_PCM_SLOT_DEFAULT_CONFIG(I2S_SLOT_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
    .gpio_cfg = {
        .ws = I2S_MIC_WORD_SELECT,
        .bclk = I2S_MIC_SERIAL_CLOCK,
        .din = I2S_MIC_SERIAL_DATA,
        .dout = I2S_GPIO_UNUSED,
        .mclk = I2S_GPIO_UNUSED,
    },
};

void microphone_setup(i2s_chan_handle_t *microphone_handle)
{
    ESP_ERROR_CHECK(i2s_new_channel(&microphone_chan_cfg, NULL, microphone_handle));
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(*microphone_handle, &microphone_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(*microphone_handle));
}

void microphone_read(i2s_chan_handle_t microphone_handle, int16_t *mic_buffer)
{
    ESP_ERROR_CHECK(i2s_channel_read(microphone_handle, mic_buffer, CONFIG_MICROPHONE_RECV_BUF_SIZE, NULL, CONFIG_MAX_TIMEOUT));
}