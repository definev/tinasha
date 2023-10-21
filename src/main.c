#include <microphone.h>
#include <speaker.h>

i2s_chan_handle_t microphone_handle;
i2s_chan_handle_t speaker_handle;

void setup(i2s_chan_handle_t *microphone_handle, i2s_chan_handle_t *speaker_handle)
{
    setup_microphone(microphone_handle);
    setup_speaker(speaker_handle);
}

void app_main()
{
    setup(&microphone_handle, &speaker_handle);
}