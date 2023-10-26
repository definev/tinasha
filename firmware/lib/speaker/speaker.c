#include "sdkconfig.h"
#include "speaker.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s_pdm.h"
#include "driver/i2s_common.h"
#include "esp_err.h"

i2s_chan_config_t speaker_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
i2s_pdm_tx_config_t speaker_std_cfg = {
    .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(48000),
    .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
    .gpio_cfg = {
        .clk = I2S_SPEAKER_SERIAL_CLOCK,
        .dout = I2S_SPEAKER_SERIAL_DATA,
    },
};

void speaker_setup(i2s_chan_handle_t *speaker_handle)
{
    /* Allocate a new TX channel and get the handle of this channel */
    ESP_ERROR_CHECK(i2s_new_channel(&speaker_chan_cfg, speaker_handle, NULL));
    /* Initialize the I2S channel in standard mode */
    ESP_ERROR_CHECK(i2s_channel_init_pdm_tx_mode(*speaker_handle, &speaker_std_cfg));
    /* Enable the I2S channel */
    ESP_ERROR_CHECK(i2s_channel_enable(*speaker_handle));
}

void speaker_write(i2s_chan_handle_t speaker_handle, int16_t *data)
{
    ESP_ERROR_CHECK(i2s_channel_write(speaker_handle, data, CONFIG_MICROPHONE_RECV_BUF_SIZE, NULL, CONFIG_MAX_TIMEOUT));
}
