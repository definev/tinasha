#ifndef VOICE_TO_SERVER_H
#define VOICE_TO_SERVER_H

#include "app/i2s.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef struct
{
    size_t size;
    char data[I2S_BUFFER_SIZE * I2S_SIZE_PER_SAMPLE];
} voice_to_server_data_t;

typedef void (*voice_to_server_callback_t)(voice_to_server_data_t data);

typedef struct
{
    QueueHandle_t queue;
    voice_to_server_callback_t callback;
} voice_to_server_handle_t;

void voice_to_server_send(voice_to_server_handle_t handle, const char *data, size_t size);

void voice_to_server_task(void *pvParameters);

void voice_to_server_setup();

#endif