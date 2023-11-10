idf_build_get_property(target IDF_TARGET)

# Always compiled source files
set(srcs
    "app_i2s/i2s_common.c"
    "app_microphone/microphone.c"
    "app_speaker/speaker.c"
    "app_voice_to_server/voice_to_server.c"
    "app_voice_to_server/udp.c"
    "app_voice_to_server/ws.c"
    "app_wifi_helper/wifi_helper.c"
    "app_tinasha_os/tcp_server.c"
    "app_tinasha_os/app_header.c")

# Always included headers
set(includes "app_i2s/include"
             "app_microphone/include"
             "app_speaker/include"
             "app_voice_to_server/include"
             "app_wifi_helper/include"
             "app_tinasha_os/include")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${includes}
                    PRIV_REQUIRES efuse 
                                  esp_timer 
                                  esp_wifi 
                                  nvs_flash 
                                  driver 
                                  esp_websocket_client
                                  esp_event
                                  esp_system)
