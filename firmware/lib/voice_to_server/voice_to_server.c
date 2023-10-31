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


void voice_to_server_task(void *pvParameters)
{
    voice_to_server_handle_t *voice_to_server = (voice_to_server_handle_t *)pvParameters;
    int16_t *data;
    while (1)
    {
        if (xQueueReceive(voice_to_server->queue, &data, portMAX_DELAY))
        {
            ESP_LOGI(TAG, "Receving data : %d bytes", sizeof(data));
            voice_to_server->callback(data, sizeof(data));
        }
        vTaskDelay(1);
    }
}

void voice_to_server_send(voice_to_server_handle_t handle, const char *data, size_t size)
{
    xQueueSend(handle.queue, data, portMAX_DELAY);
}