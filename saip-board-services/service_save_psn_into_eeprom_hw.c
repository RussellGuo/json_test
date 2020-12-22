#include "semantic_api.h"
#include "gd32e10x.h"
#include "cmsis_os2.h"


void ReplyToSavePsnIntoEeprom(
    res_error_code_t *error_code, const uint8_t *psn_byte_array, bool *return_value, serial_datagram_item_t seq)
{
    *error_code = ERR_NO_IMPL;
    *return_value = false;
    (void)seq;
    (void)psn_byte_array;
}
