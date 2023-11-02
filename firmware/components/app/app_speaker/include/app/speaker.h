#ifndef SPEAKER_H
#define SPEAKER_H

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"
#include "driver/i2s_common.h"

#include "sdkconfig.h"

/* Speaker configuration */

/* Speaker GPIO PIN definitions */
// #define I2S_SPEAKER_MCLK GPIO_NUM_3              // MCLK

void speaker_setup(i2s_chan_handle_t *speaker_handle, int32_t *wav_data);

void speaker_write(i2s_chan_handle_t speaker_handle, char *data, size_t *bytes_written);

#endif // SPEAKER_H