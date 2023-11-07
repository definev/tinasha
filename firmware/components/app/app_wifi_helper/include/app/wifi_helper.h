#ifndef WIFI_HELPER_H
#define WIFI_HELPER_H

#include "freertos/FreeRTOS.h"
#include "esp_err.h"

#include "lwip/sockets.h"

#define WIFI_HELPER_NETIF_DESC_STA "Tinasha STA"
/* The examples use WiFi configuration that you can set via project configuration menu

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define WIFI_MAXIMUM_RETRY 5

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

typedef struct
{
   struct sockaddr_storage ip_addr;
} wifi_helper_handle_t;

esp_err_t wifi_helper_connect(wifi_helper_handle_t *status);

#endif