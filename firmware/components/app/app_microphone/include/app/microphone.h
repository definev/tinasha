#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"

#define MICROPHONE_DATA_WIDTH I2S_DATA_BIT_WIDTH_16BIT
#define MICROPHONE_SLOT_MODE I2S_SLOT_MODE_STEREO

/* Microphone GPIO PIN definitions */

void microphone_setup(i2s_chan_handle_t *handle);

void microphone_read(i2s_chan_handle_t handler, void *mic_buffer, size_t buffer_size, size_t *bytes_read);

#endif // MICROPHONE_H