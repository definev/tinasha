#include "freertos/FreeRTOS.h"
#include "esp_websocket_client.h"

#define VTS_WS_URI "ws://192.168.1.2:8080/ws"

typedef struct
{
    int16_t *data;
    uint32_t len;
} voice_data_t;

voice_data_t voice_to_server_data_from_raw(int16_t *raw, uint32_t len);

typedef struct
{
    esp_websocket_client_handle_t client;
    QueueHandle_t queue;
} voice_to_server_handle_t;

void voice_to_server_task(void *pvParameters);

void setup_voice_to_server(voice_to_server_handle_t *handle);

voice_to_server_handle_t voice_to_server_create(esp_websocket_client_config_t *config, QueueHandle_t queue);

esp_err_t voice_to_server_init(voice_to_server_handle_t *handle);

void voice_to_server_send(esp_websocket_client_handle_t wsHandler, voice_data_t data);