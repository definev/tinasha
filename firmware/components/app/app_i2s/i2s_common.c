#include "app/i2s_common.h"

#include "driver/i2s.h"

void app_i2s_warmup()
{
    i2s_config_t I2S_CONFIG = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = I2S_BUFFER_SIZE,
        .tx_desc_auto_clear = true,
        .fixed_mclk = I2S_PIN_NO_CHANGE}; // mostly set by needs of microphone

    i2s_pin_config_t PIN_CONFIG = {
        .bck_io_num = I2S_BCK_PIN,
        .ws_io_num = I2S_WS_PIN,
        .data_out_num = I2S_SD_OUT,
        .data_in_num = I2S_SD_IN};

    i2s_driver_install(I2S_PORT, &I2S_CONFIG, 0, NULL);
    i2s_set_pin(I2S_PORT, &PIN_CONFIG);
}