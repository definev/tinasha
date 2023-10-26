#include "sdkconfig.h"
#include "microphone.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s_pdm.h"
#include "driver/i2s_common.h"
#include "esp_err.h"

i2s_chan_config_t microphone_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
i2s_pdm_rx_config_t microphone_cfg = {
    .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(CONFIG_SAMPLE_RATE),
    .slot_cfg = I2S_PDM_RX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
    .gpio_cfg = {
        .clk = I2S_MIC_SERIAL_CLOCK,
        .din = I2S_MIC_SERIAL_DATA,
    },
};

void microphone_setup(i2s_chan_handle_t *microphone_handle)
{
    ESP_ERROR_CHECK(i2s_new_channel(&microphone_chan_cfg, NULL, microphone_handle));
    ESP_ERROR_CHECK(i2s_channel_init_pdm_rx_mode(*microphone_handle, &microphone_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(*microphone_handle));
}

void microphone_read(i2s_chan_handle_t microphone_handle, int16_t *mic_buffer)
{
    ESP_ERROR_CHECK(i2s_channel_read(microphone_handle, mic_buffer, CONFIG_MICROPHONE_RECV_BUF_SIZE, NULL, CONFIG_MAX_TIMEOUT));
}