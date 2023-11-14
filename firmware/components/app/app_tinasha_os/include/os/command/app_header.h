#ifndef APP_COMMAND_HEADER_H
#define APP_COMMAND_HEADER_H

#include "freertos/FreeRTOS.h"

typedef enum
{
    /*
    header[0] = 0xAA receive audio
    header[1:2] = mic timeout after playing (ms)
    header[3] = volume
    header[4] = none
    header[5] = none
    */
    HEADER_TYPE_RECEIVE_WAV = 0xAA,

    /*
    header[0] = 0xBB
    header[1] = none
    header[2] = none
    header[3] = volume
    header[4] = none
    header[5] = none
    */
    HEADER_TYPE_ADJUST_VOLUME = 0xBB,

    /*
    header[0] = 0xCC
    header[1:2] = mic timeout after playing (ms)
    header[3] = none
    header[4] = none
    header[5] = none
    */
    HEADER_TYPE_MICROPHONE_TIMEOUT = 0xDD,
} header_type_t;

void command_header_log(uint8_t *header);

header_type_t command_header_get_type(uint8_t *header);

void command_header_parse_timeout(uint8_t *header, uint16_t *timeout);
void command_header_parse_volume(uint8_t *header, volatile uint8_t *volume);

#endif