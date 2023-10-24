#ifndef SPEAKER_H
#define SPEAKER_H

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"
#include "driver/i2s_common.h"

/* Speaker configuration */

/* Speaker GPIO PIN definitions */
#define I2S_SPEAKER_LEFT_RIGHT_CLOCK GPIO_NUM_40 // LRCLK
#define I2S_SPEAKER_SERIAL_CLOCK GPIO_NUM_41     // SCLK
#define I2S_SPEAKER_SERIAL_DATA GPIO_NUM_42      // SDIN
#define I2S_SPEAKER_MCLK GPIO_NUM_43             // MCLK

void setup_speaker(i2s_chan_handle_t *speaker_handle);

void write_speaker(i2s_chan_handle_t speaker_handle, int16_t *data, size_t len);

#endif // SPEAKER_H