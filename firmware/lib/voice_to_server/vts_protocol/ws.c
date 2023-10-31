#include "i2s/i2s.h"
#include "voice_to_server.h"

#include "freertos/FreeRTOS.h"

#include "esp_websocket_client.h"
#include "esp_log.h"

static const char *TAG = "vts_ws";

static esp_websocket_client_config_t voice_to_server_client_cfg = {
    .uri = CONFIG_WS_URI,
    .disable_auto_reconnect = false,
    .buffer_size = I2S_BUFFER_SIZE * I2S_SIZE_PER_SAMPLE,
    .reconnect_timeout_ms = 3000,
    .network_timeout_ms = 30000000,
};
esp_websocket_client_handle_t client;

void voice_to_server_ws_setup(void)
{
    client = esp_websocket_client_init(&voice_to_server_client_cfg);
    esp_websocket_client_start(client);
}

void voice_to_server_ws_callback(char *data, size_t size)
{
    if (client == NULL)
    {
        ESP_LOGE(TAG, "Websocket client is not initialized");
        voice_to_server_ws_setup();
    }

    if (client != NULL && esp_websocket_client_is_connected(client))
    {
        esp_websocket_client_send_bin(client, data, size, portMAX_DELAY);
    }
}
