#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s_std.h"

/* Microphone GPIO PIN definitions */

void microphone_setup(i2s_chan_handle_t *microphone_handle);

void microphone_read(i2s_chan_handle_t microphone_handle, char *mic_buffer, size_t *bytes_read);

#endif // MICROPHONE_H