#include "voice_to_server.h"
#include "i2s/i2s.h"
#include "sdkconfig.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"

static const char *TAG = "voice_to_server";

esp_websocket_client_config_t voice_to_server_client_cfg = {
    .uri = WS_URI,
    .disable_auto_reconnect = false,
    .buffer_size = I2S_BUFFER_SIZE * sizeof(int32_t),
    .reconnect_timeout_ms = 30000000,
    .network_timeout_ms = 3000000,
};

void voice_to_server_task(void *pvParameters)
{
    voice_to_server_handle_t *voice_to_server = (voice_to_server_handle_t *)pvParameters;
    int16_t *data;
    while (1)
    {
        if (xQueueReceive(voice_to_server->queue, &data, portMAX_DELAY))
        {
            ESP_LOGI(TAG, "Receving data : %d bytes", sizeof(data));
            esp_websocket_client_send_bin(voice_to_server->client, (const char *)data, sizeof(data), portMAX_DELAY);
        }
        vTaskDelay(1);
    }
}

esp_err_t voice_to_server_init(voice_to_server_handle_t *handle)
{
    esp_websocket_client_handle_t client = esp_websocket_client_init(&voice_to_server_client_cfg);
    voice_to_server_handle_t new_handle = {
        .client = client,
        .queue = xQueueCreate(10, sizeof(int16_t *)),
    };

    *handle = new_handle;
    esp_err_t err = esp_websocket_client_start(handle->client);
    if (err != ESP_OK)
    {
        ESP_LOGE("voice_to_server", "Error starting websocket client: %s", esp_err_to_name(err));
        return err;
    }
    if (handle->queue == NULL)
    {
        ESP_LOGE("voice_to_server", "Queue is not initialized");
        return ESP_ERR_INVALID_ARG;
    }
    return ESP_OK;
}

void voice_to_server_send(voice_to_server_handle_t handle, const char *data, size_t size)
{
    xQueueSend(handle.queue, data, portMAX_DELAY);
}