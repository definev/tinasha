#ifndef VTS_WS_H
#define VTS_WS_H

#include "app/i2s_common.h"
#include "freertos/FreeRTOS.h"

#define VTS_WS_BUFFER_SIZE (I2S_BUFFER_SIZE * I2S_SIZE_PER_SAMPLE)

typedef struct
{
    char *uri;
    size_t buffer_size;
} vts_ws_config_t;

void voice_to_server_ws_setup(vts_ws_config_t config);

void voice_to_server_ws_callback(const char *data, size_t size);

#endif