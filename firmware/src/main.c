#include "microphone.h"
#include "speaker.h"
#include "voice_to_server.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

i2s_chan_handle_t microphone_handle;
i2s_chan_handle_t speaker_handle;
voice_to_server_handle_t voice_to_server_handle;

void setup()
{
    // setup_microphone(&microphone_handle);
    // setup_speaker(&speaker_handle);
    // setup_voice_to_server(&voice_to_server_handle);
}

void repeat_microphone(void *arg)
{
    while (1)
    {
        // int16_t *samples = read_microphone(&microphone_handle);
        printf("Read %d samples\n", SAMPLE_SIZE);

        // Send over websocket
        // voice_data_t data = voice_to_server_data_from_raw(samples, SAMPLE_SIZE);
        // xQueueSend(voice_to_server_handle.queue, &data, portMAX_DELAY);
    }
}

void app_main()
{
    printf("[APP] Startup..");
    printf("[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    printf("[APP] IDF version: %s", esp_get_idf_version());
    // setup(&microphone_handle, &speaker_handle, &voice_to_server_handle);
    xTaskCreatePinnedToCore(&repeat_microphone, "repeat_microphone", 4096, NULL, 1, NULL, 0);
}