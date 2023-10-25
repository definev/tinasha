#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"

#define CONFIG_SAMPLE_RATE 44100
#define CONFIG_BIT_SAMPLE 16
#define NUM_CHANNELS 1
#define SAMPLE_SIZE (CONFIG_BIT_SAMPLE * 1024)
#define BYTE_RATE (CONFIG_SAMPLE_RATE * (CONFIG_BIT_SAMPLE / 8)) * NUM_CHANNELS

/* Microphone GPIO PIN definitions */
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_11     // SCK
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_10 // WS
#define I2S_MIC_SERIAL_DATA GPIO_NUM_12      // SD

void microphone_setup(i2s_chan_handle_t *microphone_handle);

void microphone_read(i2s_chan_handle_t microphone_handle, int16_t *mic_buffer);

#endif // MICROPHONE_H