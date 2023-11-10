#ifndef SPEAKER_H
#define SPEAKER_H

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"

#define wav_size_t int32_t

wav_size_t *speaker_init_buffer();

void speaker_append_tcp_to_wav(uint8_t *tcp_data, size_t tcp_data_size, wav_size_t *wav_data, size_t *wav_data_size, uint8_t volume);

void speaker_setup(void);

void speaker_write(const char *data, size_t size, size_t *bytes_written);

#endif // SPEAKER_H