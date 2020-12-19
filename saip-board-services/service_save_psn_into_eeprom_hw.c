#include "semantic_api.h"
#include "gd32e10x.h"
#include "cmsis_os2.h"

void ReplyToSavePsnIntoEeprom(
    res_error_code_t *error_code,
    const uint8_t *data_array,
    serial_datagram_item_t seq)
{
    *error_code = ERR_NO_IMPL;
    (void)seq;
    (void)data_array;
}
