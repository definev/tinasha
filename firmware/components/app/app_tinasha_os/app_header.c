#include "os/command/app_header.h"

#include "freertos/FreeRTOS.h"

#include "esp_log.h"

static const char *TAG = "app_header";

void command_header_log(uint8_t *header)
{
    ESP_LOGI(TAG, "(%d, %d, %d, %d, %d, %d)", header[0], header[1], header[2], header[3], header[4], header[5]);
}

void command_header_parse_timeout(uint8_t *header, uint16_t *timeout)
{
    *timeout = (uint16_t)(header[1] << 8 | header[2]);
    ESP_LOGI(TAG, "timeout: %d", *timeout);
}

void command_header_parse_volume(uint8_t *header, volatile uint8_t *volume)
{
    uint8_t value = header[3];
    if (value > 20 || value < 8)
        return;
    *volume = value;
    ESP_LOGI(TAG, "volume: %d", *volume);
}

header_type_t command_header_get_type(uint8_t *header)
{
    return (header_type_t)header[0];
}