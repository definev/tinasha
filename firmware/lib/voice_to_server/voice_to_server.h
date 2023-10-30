#ifndef VOICE_TO_SERVER_H
#define VOICE_TO_SERVER_H

#include "freertos/FreeRTOS.h"
#include "esp_websocket_client.h"

#define WS_URI "ws://192.168.1.6:8080"

typedef struct
{
    esp_websocket_client_handle_t client;
    QueueHandle_t queue;
} voice_to_server_handle_t;

esp_err_t voice_to_server_init(voice_to_server_handle_t *handle);

void voice_to_server_send(voice_to_server_handle_t handle, const char *data, size_t size);

void voice_to_server_task(void *pvParameters);

#endif