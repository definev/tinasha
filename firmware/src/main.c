#include "appconfig.h"
#include "microphone.h"
#include "speaker.h"
// #include "voice_to_server.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

i2s_chan_handle_t microphone_handle;
i2s_chan_handle_t speaker_handle;

int16_t samples[MICROPHONE_RECV_BUF_SIZE] = {0};
// voice_to_server_handle_t voice_to_server_handle;

void setup()
{
    microphone_setup(&microphone_handle);
    speaker_setup(&speaker_handle);
    // setup_voice_to_server(&voice_to_server_handle);
}

void repeat_microphone(void *arg)
{
    vTaskDelay(1);
    while (1)
    {
        microphone_read(microphone_handle, samples);
        speaker_write(speaker_handle, samples);
        // Send over websocket
        // voice_data_t data = voice_to_server_data_from_raw(samples, SAMPLE_SIZE);
        // xQueueSend(voice_to_server_handle.queue, &data, portMAX_DELAY);
    }
}

void app_main()
{
    setup();
    xTaskCreatePinnedToCore(&repeat_microphone, "repeat_microphone", 4096, NULL, 1, NULL, 0);
}