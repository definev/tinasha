#ifndef SPEAKER_H
#define SPEAKER_H

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"

#include "sdkconfig.h"

void speaker_setup(i2s_chan_handle_t *speaker_handle, int32_t *wav_data);

void speaker_write(i2s_chan_handle_t speaker_handle, char *data, size_t *bytes_written);

    #endif // SPEAKER_H