#include <string.h>

#include "app/wifi_helper.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

static const char *TAG = "wifi_station";

static EventGroupHandle_t wifi_event_group;

static int s_retry_num = 0;

// event handler for wifi events
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    wifi_helper_handle_t *handle = (wifi_helper_handle_t *)arg;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "Connecting to AP...");
        esp_err_t err = esp_wifi_connect();
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to connect to AP: %s", esp_err_to_name(err));
            handle->connected = false;
        }
        else
        {
            ESP_LOGI(TAG, "Connected to AP");
            handle->connected = true;
        }
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < WIFI_MAXIMUM_RETRY)
        {
            ESP_LOGI(TAG, "Reconnecting to AP...");
            esp_err_t err = esp_wifi_connect();
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to reconnect to AP: %s", esp_err_to_name(err));
                handle->connected = false;
                s_retry_num++;
            }
            else
            {
                ESP_LOGI(TAG, "Reconnected to AP");
                handle->connected = true;
            }
        }
        else
        {
            xEventGroupSetBits(wifi_event_group, WIFI_FAIL_BIT);
            handle->connected = false;
        }
    }
}

// event handler for ip events
static void ip_event_handler(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data)
{
    wifi_helper_handle_t *handle = (wifi_helper_handle_t *)arg;
    if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "STA IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        handle->ip_addr = event->ip_info.ip.addr;
    }
}

// connect to wifi and return the result
esp_err_t wifi_helper_connect(wifi_helper_handle_t *handle)
{
    int status = WIFI_FAIL_BIT;

    // create wifi station in the wifi driver
    esp_netif_t *netif = esp_netif_create_default_wifi_sta();
    esp_netif_set_hostname(netif, WIFI_HELPER_NETIF_DESC_STA);

    // setup wifi station with the default wifi configuration
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /** EVENT LOOP CRAZINESS **/
    wifi_event_group = xEventGroupCreate();

    esp_event_handler_instance_t wifi_handler_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        (void *)handle,
                                                        &wifi_handler_event_instance));

    esp_event_handler_instance_t got_ip_event_instance;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &ip_event_handler,
                                                        (void *)handle,
                                                        &got_ip_event_instance));

    /** START THE WIFI DRIVER **/
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false,
            },
        },
    };

    // set the wifi controller to be a station
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    // set the wifi config
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // start the wifi driver
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "STA initialization complete");

    /** NOW WE WAIT **/
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(TAG, "Connected to ap");
        status = WIFI_CONNECTED_BIT;
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(TAG, "Failed to connect to ap");
        status = WIFI_FAIL_BIT;
    }

    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, got_ip_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler_event_instance));
    vEventGroupDelete(wifi_event_group);

    const char *hostname = NULL;
    esp_netif_get_hostname(netif, &hostname);
    ESP_LOGI(TAG, "Hostname: %s", hostname);

    return status;
}

// // connect to the server and return the result
// esp_err_t connect_tcp_server(void)
// {
// 	struct sockaddr_in serverInfo = {0};
// 	char readBuffer[1024] = {0};

// 	serverInfo.sin_family = AF_INET;
// 	serverInfo.sin_addr.s_addr = 0x0100007f;
// 	serverInfo.sin_port = htons(12345);

// 	int sock = socket(AF_INET, SOCK_STREAM, 0);
// 	if (sock < 0)
// 	{
// 		ESP_LOGE(TAG, "Failed to create a socket..?");
// 		return TCP_FAILURE;
// 	}

// 	if (connect(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) != 0)
// 	{
// 		ESP_LOGE(TAG, "Failed to connect to %s!", inet_ntoa(serverInfo.sin_addr.s_addr));
// 		close(sock);
// 		return TCP_FAILURE;
// 	}

// 	ESP_LOGI(TAG, "Connected to TCP server.");
// 	bzero(readBuffer, sizeof(readBuffer));
//     int r = read(sock, readBuffer, sizeof(readBuffer)-1);
//     for(int i = 0; i < r; i++) {
//         putint8_t(readBuffer[i]);
//     }

//     if (strcmp(readBuffer, "HELLO") == 0)
//     {
//     	ESP_LOGI(TAG, "WE DID IT!");
//     }

//     return TCP_SUCCESS;
// }