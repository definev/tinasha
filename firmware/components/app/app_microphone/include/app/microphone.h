#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "app/i2s_common.h"
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"

#define MICROPHONE_DATA_WIDTH I2S_DATA_BIT_WIDTH_16BIT
#define MICROPHONE_SLOT_MODE I2S_SLOT_MODE_MONO

typedef struct 
{
    int16_t buffer[I2S_BUFFER_SIZE];
    size_t buffer_size;
    size_t bytes_read;
    uint64_t timeout;
} microphone_handle_t;

void microphone_setup(microphone_handle_t *handle);

void microphone_read(int16_t *data, size_t *bytes_read);

#endif // MICROPHONE_H