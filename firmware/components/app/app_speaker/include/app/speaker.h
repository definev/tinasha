#ifndef SPEAKER_H
#define SPEAKER_H

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"

void speaker_setup(void);

void speaker_write(char *data, size_t *bytes_written);

#endif // SPEAKER_H