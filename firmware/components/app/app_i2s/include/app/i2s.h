#ifndef I2SAPPCONFIG_H
#define I2SAPPCONFIG_H

#include "driver/gpio.h"
#include "driver/i2s.h"

#define I2S_MASTER_CLOCK GPIO_NUM_9

#define I2S_MIC_SERIAL_DATA GPIO_NUM_17 // SD
#define I2S_SERIAL_CLOCK GPIO_NUM_9
#define I2S_WORD_SELECT GPIO_NUM_15
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_13 // SDIN

#define I2S_MIC_PORT I2S_NUM_0

#define I2S_DMA_DESC_NUM 10   // Same with I2S DMA buffer count
#define I2S_DMA_FRAME_NUM 1024 // Same with I2S DMA buffer size
#define I2S_BUFFER_SIZE I2S_DMA_FRAME_NUM
#define I2S_SAMPLE_BITS 16
#define I2S_SIZE_PER_SAMPLE (I2S_SAMPLE_BITS / 8)
#define I2S_SAMPLE_RATE 16000
#define I2S_BIT_PER_SAMPLE I2S_BITS_PER_SAMPLE_16BIT
#define I2S_MONO_OR_STEREO I2S_SLOT_MODE_STEREO

#endif