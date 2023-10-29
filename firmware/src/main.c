#include "sdkconfig.h"
#include "i2s/i2s.h"
#include "wifi_helper.h"
#include "voice_to_server.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_event.h"
#include "driver/i2s_std.h"
#include "driver/i2s_types.h"

#define millis() (esp_timer_get_time() / 1000)

const char *MAIN_TAG = "main";

i2s_chan_handle_t microphone_handle;
i2s_chan_handle_t speaker_handle;
voice_to_server_handle_t voice_to_server_handle;

#define MAX_ALLOWED_OFFSET 16000
#define MIC_OFFSET_AVERAGING_FRAMES 1
#define VAD_MIC_EXTEND 5000 // ensure there's always another 5s after last VAD detected by server to avoid cutting off while talking

volatile bool isPlaying = false;
uint32_t mic_timeout = 0;

int16_t mic_buff[I2S_CHUNK_SIZE];
int16_t converted_mic_buff[I2S_CHUNK_SIZE];

size_t bytes_read;
size_t bytes_written;

void setup_i2s()
{
    i2s_chan_config_t i2s_chan_config = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    i2s_std_config_t i2s_std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = I2S_SERIAL_CLOCK,
            .ws = I2S_WORD_SELECT,
            .din = I2S_MIC_SERIAL_DATA,
            .dout = I2S_SPEAKER_SERIAL_DATA,
            .invert_flags = {
                .bclk_inv = false,
                .ws_inv = false,
                .mclk_inv = false,
            },
        },
    };

    i2s_new_channel(&i2s_chan_config, NULL, &microphone_handle);
    i2s_channel_init_std_mode(microphone_handle, &i2s_std_cfg);
    // i2s_channel_init_std_mode(speaker_handle, &i2s_std_cfg);
    i2s_channel_enable(microphone_handle);
    // i2s_channel_enable(speaker_handle);
}

void setup()
{
    setup_i2s();
    setup_voice_to_server(&voice_to_server_handle);
}

void repeat_microphone(void *arg)
{
    ESP_LOGI(MAIN_TAG, "repeat_microphone task started");

    // int64_t sum = 0;
    // int16_t shifted_value = 0;

    // i2s_channel_read(microphone_handle, mic_buff, sizeof(mic_buff), &bytes_read, portMAX_DELAY);
    // int CURRENT_CHUNK_SIZE = sizeof(mic_buff) / sizeof(mic_buff[0]);
    // for (int i = 0; i < CURRENT_CHUNK_SIZE; i++)
    // {
    //     shifted_value = (int16_t)mic_buff[i] >> 14;
    //     sum += shifted_value;
    // }
    // ESP_LOGI(MAIN_TAG, "sum: %lld", sum);

    // int16_t offset = sum / CURRENT_CHUNK_SIZE;

    // ESP_LOGI(MAIN_TAG, "Calculate mic offset: %d", offset);
    // if (abs(offset) > MAX_ALLOWED_OFFSET)
    // {
    //     ESP_LOGE(MAIN_TAG, "Offset too large, set to zero: %d", offset);
    //     offset = 0;
    // }

    while (1)
    {
        // if (mic_timeout < millis() && read_successed)
        // {
        //     mic_timeout = millis() + 300000;
        //     ESP_LOGI(MAIN_TAG, "Timeout reached, stopping microphone");
        //     goto changeState;
        // }
        bytes_read = 0;

        i2s_channel_read(microphone_handle, mic_buff, sizeof(mic_buff), &bytes_read, portMAX_DELAY);
        printf("bytes_read: %d\n", bytes_read);
        // TODO: Sending to server
        for (int i = 0; i < I2S_CHUNK_SIZE; i++)
        {
            converted_mic_buff[i] = (int16_t)mic_buff[i] >> 1;
        }
        voice_to_server_send(voice_to_server_handle.client, converted_mic_buff);

        vTaskDelay(1);
    }
}

void app_main()
{
    nvs_flash_init();
    esp_netif_init();
    wifi_helper_sta_connect();
    setup();

    // xTaskCreatePinnedToCore(&voice_to_server_task, "voice_to_server_task", 4096, handle, 1, NULL, 1);
    xTaskCreatePinnedToCore(&repeat_microphone, "repeat_microphone", 4096, NULL, 1, NULL, 0);
}