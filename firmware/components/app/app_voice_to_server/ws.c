#include "app/i2s.h"
#include "app/vts_protocol/ws.h"
#include "app/voice_to_server.h"

#include "freertos/FreeRTOS.h"

#include "esp_websocket_client.h"
#include "esp_log.h"

static const char *TAG = "vts_ws";

esp_websocket_client_handle_t client;

void voice_to_server_ws_setup(vts_ws_config_t config)
{
    esp_websocket_client_config_t voice_to_server_client_cfg = {
        .uri = config.uri,
        .buffer_size = config.buffer_size,
    };

    client = esp_websocket_client_init(&voice_to_server_client_cfg);
    esp_websocket_client_start(client);
}

void voice_to_server_ws_callback(char *data, size_t size)
{
    if (client == NULL)
    {
        ESP_LOGE(TAG, "Websocket client is not initialized");
        return;
    }

    if (esp_websocket_client_is_connected(client))
    {
        esp_websocket_client_send_bin(client, data, size, portMAX_DELAY);
    }
}
