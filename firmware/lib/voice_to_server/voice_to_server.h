#include "freertos/FreeRTOS.h"
#include "esp_websocket_client.h"

typedef struct
{
    esp_websocket_client_handle_t client;
    QueueHandle_t queue;
} voice_to_server_handle_t;

void voice_to_server_task(void *pvParameters);

void setup_voice_to_server(voice_to_server_handle_t *handler);

voice_to_server_handle_t voice_to_server_create(esp_websocket_client_config_t *config, QueueHandle_t queue);

esp_err_t voice_to_server_init(voice_to_server_handle_t *handle);

void voice_to_server_send(esp_websocket_client_handle_t wsHandler, int16_t *data);