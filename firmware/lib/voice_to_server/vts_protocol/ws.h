#ifndef VTS_WS_H
#define VTS_WS_H

#include "freertos/FreeRTOS.h"

void voice_to_server_ws_callback(void *data, size_t size);
void voice_to_server_ws_setup(void);

#endif