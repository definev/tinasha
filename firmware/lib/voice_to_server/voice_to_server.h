#include "freertos/FreeRTOS.h"
#include "esp_websocket_client.h"

#define WS_URI "ws://192.168.1.6:8080"

typedef struct
{
    esp_websocket_client_handle_t client;
    QueueHandle_t queue;
} voice_to_server_handle_t;

void voice_to_server_task(void *pvParameters);

void setup_voice_to_server(voice_to_server_handle_t *handler);

voice_to_server_handle_t voice_to_server_create(esp_websocket_client_config_t *config, QueueHandle_t queue);

esp_err_t voice_to_server_init(voice_to_server_handle_t *handle);

void voice_to_server_send(esp_websocket_client_handle_t wsHandler, const char *data, size_t size);