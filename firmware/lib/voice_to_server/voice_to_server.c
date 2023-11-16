#include "voice_to_server.h"

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


esp_websocket_client_config_t voice_to_server_client_cfg = {
    .uri = VTS_WS_URI,
    .disable_auto_reconnect = true,
};

voice_data_t voice_to_server_data_from_raw(int16_t *raw, uint32_t len)
{
    voice_data_t data = {
        .data = raw,
        .len = len,
    };
    return data;
}

void voice_to_server_task(void *pvParameters)
{
    voice_to_server_handle_t *voice_to_server = (voice_to_server_handle_t *)pvParameters;
    voice_data_t data;
    while (1)
    {
        if (xQueueReceive(voice_to_server->queue, &data, portMAX_DELAY))
        {
            voice_to_server_send(voice_to_server->client, data);
        }
    }
}

void setup_voice_to_server(voice_to_server_handle_t *handle)
{
    voice_to_server_handle_t newHandle = voice_to_server_create(
        &voice_to_server_client_cfg,
        xQueueCreate(10, sizeof(voice_data_t)));
    *handle = newHandle;
    voice_to_server_init(handle);
    xTaskCreatePinnedToCore(&voice_to_server_task, "voice_to_server_task", 4096, handle, 1, NULL, 1);
}

voice_to_server_handle_t voice_to_server_create(esp_websocket_client_config_t *config, QueueHandle_t queue)
{
    esp_websocket_client_handle_t client = esp_websocket_client_init(config);
    voice_to_server_handle_t handle = {
        .client = client,
        .queue = queue,
    };
    return handle;
}

esp_err_t voice_to_server_init(voice_to_server_handle_t *handle)
{
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

void voice_to_server_send(esp_websocket_client_handle_t wsHandler, voice_data_t data)
{
    esp_websocket_client_send_bin(wsHandler, (char *)data.data, data.len, portMAX_DELAY);
}