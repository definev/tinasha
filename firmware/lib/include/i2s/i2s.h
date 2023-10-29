#ifndef I2SAPPCONFIG_H
#define I2SAPPCONFIG_H

#include "driver/gpio.h"

#define I2S_MASTER_CLOCK GPIO_NUM_9

#define I2S_MIC_SERIAL_DATA GPIO_NUM_10 // SD
#define I2S_WORD_SELECT GPIO_NUM_11
#define I2S_SERIAL_CLOCK GPIO_NUM_12
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_13 // SDIN

#define I2S_MIC_PORT I2S_NUM_0

#define I2S_DMA_DESC_NUM 6    // Same with I2S DMA buffer count
#define I2S_DMA_FRAME_NUM 480 // Same with I2S DMA buffer size
#define I2S_BUFFER_SIZE 6 * I2S_DMA_FRAME_NUM
#define I2S_SAMPLE_RATE 16000

#endif