#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"

#define MICROPHONE_DATA_WIDTH I2S_DATA_BIT_WIDTH_16BIT
#define MICROPHONE_SLOT_MODE I2S_SLOT_MODE_MONO

/* Microphone GPIO PIN definitions */

void microphone_setup(void);

void microphone_read(int16_t *data, size_t *bytes_read);

#endif // MICROPHONE_H