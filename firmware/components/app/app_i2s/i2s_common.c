#include "app/i2s_common.h"

#include "driver/i2s.h"

void app_i2s_warmup()
{
    i2s_config_t I2S_CONFIG = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_RX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
        .communication_format = I2S_COMM_FORMAT_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = I2S_DMA_DESC_NUM,
        .dma_buf_len = I2S_DMA_FRAME_NUM}; // mostly set by needs of microphone

    i2s_pin_config_t PIN_CONFIG = {
        .bck_io_num = I2S_SERIAL_CLOCK,
        .ws_io_num = I2S_WORD_SELECT,
        .data_out_num = I2S_SPEAKER_SERIAL_DATA,
        .data_in_num = I2S_MICROPHONE_SERIAL_DATA};

    i2s_driver_install(I2S_PORT, &I2S_CONFIG, 0, NULL);
    i2s_set_pin(I2S_PORT, &PIN_CONFIG);
    i2s_start(I2S_PORT);
}