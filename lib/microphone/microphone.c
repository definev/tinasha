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

int16_t i2s_readraw_buff[SAMPLE_SIZE];
size_t bytes_read;
const int WAVE_HEADER_SIZE = 44;

#define SAMPLE_CHUNK_SIZE 480                           // chosen to be 30ms (at 16kHz) for WebRTC VAD, and fit within UDP packet as int16 (480 * 2 < 1400)
int32_t mic_buffer[SAMPLE_CHUNK_SIZE];           // For raw values from I2S
int16_t converted_mic_buffer[SAMPLE_CHUNK_SIZE]; // For converted values to be sent over UDP

void setup_microphone(i2s_chan_handle_t *microphone_handle)
{
    ESP_ERROR_CHECK(i2s_new_channel(&microphone_chan_cfg, NULL, microphone_handle));
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(*microphone_handle, &microphone_rx_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(*microphone_handle));
}