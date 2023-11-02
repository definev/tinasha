#include "app/voice_to_server.h"
#include "app/i2s.h"
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

void voice_to_server_task(void *pvParameters)
{
    voice_to_server_handle_t *voice_to_server = (voice_to_server_handle_t *)pvParameters;
    voice_to_server_data_t data;
    while (1)
    {
        if (xQueueReceive(voice_to_server->queue, &data, portMAX_DELAY))
        {
            voice_to_server->callback(data);
        }
        vTaskDelay(1);
    }
}

static voice_to_server_data_t server_data;

void voice_to_server_setup()
{
    server_data.size = 0;
    for (int i = 0; i < I2S_BUFFER_SIZE * I2S_SIZE_PER_SAMPLE; i++)
    {
        server_data.data[i] = 0;
    }
}

void voice_to_server_send(voice_to_server_handle_t handle, const char *data, size_t size)
{
    ESP_LOGI(TAG, "vts_send data : %d bytes", sizeof(data));
    ESP_LOGI(TAG, "voice_to_server_send");
    for (int i = 0; i < size; i++)
    {
        server_data.data[i] = data[i];
    }
    server_data.size = size;
    vTaskDelay(pdMS_TO_TICKS(5000));
    xQueueSend(handle.queue, &server_data, portMAX_DELAY);
}