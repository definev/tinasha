#ifndef VOICE_TO_SERVER_H
#define VOICE_TO_SERVER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef void (*voice_to_server_callback_t)(void *data, size_t size);

typedef struct
{
    QueueHandle_t queue;
    voice_to_server_callback_t callback;
} voice_to_server_handle_t;

void voice_to_server_send(voice_to_server_handle_t handle, const char *data, size_t size);

void voice_to_server_task(void *pvParameters);

#endif