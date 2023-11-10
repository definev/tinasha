#ifndef I2S_COMMON_H
#define I2S_COMMON_H

#include "driver/gpio.h"
#include "driver/i2s.h"

#define I2S_SD_IN GPIO_NUM_39  // SD
#define I2S_SD_OUT GPIO_NUM_40 // SD
#define I2S_BCK_PIN GPIO_NUM_41
#define I2S_WS_PIN GPIO_NUM_42

#define I2S_PORT I2S_NUM_0

#define I2S_DMA_DESC_NUM 6    // Same with I2S DMA buffer count
#define I2S_DMA_FRAME_NUM 480 // Same with I2S DMA buffer size
#define I2S_BUFFER_SIZE I2S_DMA_FRAME_NUM
#define I2S_SAMPLE_BITS 16
#define I2S_SIZE_PER_SAMPLE (I2S_SAMPLE_BITS / 8)
#define I2S_SAMPLE_RATE 16000
#define I2S_BIT_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT
#define I2S_SPEAKER_BIT_PER_SAMPLE I2S_BITS_PER_SAMPLE_32BIT

void app_i2s_warmup();

#endif