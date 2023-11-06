// #include <stdlib.h>

// #include "sdkconfig.h"
// #include "app/i2s.h"
// #include "app/speaker.h"

// #include "freertos/FreeRTOS.h"
// #include "driver/i2s.h"
// #include "esp_err.h"
// #include "esp_log.h"
// #include "esp_heap_caps.h"

// // how many samples to load from TCP before starting playing (avoid jitter due to running out of data w/ bad wifi)
// #ifdef CONFIG_SOC_GDMA_SUPPORT_PSRAM
// int bufferThreshold = 8192;
// #else
// int bufferThreshold = 512;
// #endif

// static const char *TAG = "speaker";

// void speaker_setup(int32_t *wav_data)
// {
//     /* Initialize WAV form */
//     size_t free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
//     size_t total_psram = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);

//     ESP_LOGI(TAG, "PSRAM free: %d", free_psram);
//     ESP_LOGI(TAG, "PSRAM total: %d", total_psram);
//     ESP_LOGI(TAG, "PSRAM used: %d", total_psram - free_psram);

//     wav_data = (int32_t *)malloc((2 * 1024 * 1024) / sizeof(int32_t));
//     if (wav_data == NULL)
//     {
//         ESP_LOGE(TAG, "Memory allocation failed!");
//         while (1)
//             ;
//     }
//     else
//     {
//         ESP_LOGI(TAG, "Memory allocation successful!");
//     }
//     free_psram = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
//     ESP_LOGI(TAG, "PSRAM used: %d", total_psram - free_psram);

//     /* Initialize speaker */
//     i2s_chan_config_t speaker_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
//     ESP_ERROR_CHECK(i2s_new_channel(&speaker_chan_cfg, speaker_handle, NULL));

//     i2s_std_config_t speaker_std_cfg = {
//         .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(I2S_SAMPLE_RATE),
//         .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_BIT_PER_SAMPLE, I2S_MONO_OR_STEREO),
//         .gpio_cfg = {
//             .bclk = I2S_SERIAL_CLOCK,
//             .ws = I2S_WORD_SELECT,
//             .dout = I2S_SPEAKER_SERIAL_DATA,
//         },
//     };
//     ESP_ERROR_CHECK(i2s_channel_init_std_mode(*speaker_handle, &speaker_std_cfg));
//     ESP_ERROR_CHECK(i2s_channel_enable(*speaker_handle));
// }

// void speaker_write(char *data, size_t *bytes_written)
// {
//     ESP_ERROR_CHECK(i2s_channel_write(speaker_handle, data, I2S_BUFFER_SIZE, bytes_written, CONFIG_MAX_TIMEOUT));
// }
