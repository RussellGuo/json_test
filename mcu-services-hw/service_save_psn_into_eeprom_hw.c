/*
 * service_save_psn_into_eeprom_hw.c
 *
 * hardware-depended part of 'save PSN' service for the SAIP
 *
 *  Created on: Dec 14, 2020
 *      Author: Guo Qiang
 */

#include "semantic_api.h"
#include "gd32e10x.h"
#include "cmsis_os2.h"

#include "eeprom.h"

#include <string.h>

// PSN will save here according the EEPROM layout design from people of product line
#define PSN_OFFSET_ADDR_IN_EEPROM 4

// ReplyToSavePsnIntoEeprom, the function is in the API layer of the host-MCU communication
// protocol and runs in the protocol receiving thread. It will be called by protocol receiving thread
// (after checking the validity of the parameters), then do all the test, and store the result
// and then the receiving thread will send them to the host as a respose. Because the saving psn is executed
// under specific circumstances, it does not need to be placed in a separate thread, but directly
// in the receiving thread. In this case, it is not a problem that the receiving thread is
// blocked for tens of hundreds of milliseconds. See the documentation for details.
// parameters:
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [in]  psn_byte_array, the byte array of PSN with length PSN_BYTE_COUNT
//   [out] return value, the test result 0/1 (false/true)
//   [in]  seq, the request seq
void ReplyToSavePsnIntoEeprom(
    res_error_code_t *error_code, const uint8_t *psn_byte_array, bool *return_value, serial_datagram_item_t seq)
{
    *error_code = NO_ERROR;
    (void)seq;

#define EEPROM_IO_DELAY() osDelay(2)

    bool ret = true;

    // save each byte of input
    for (int i = 0; i < PSN_BYTE_COUNT; i++) {
        ret = ret && eeprom_byte_write(psn_byte_array[i], PSN_OFFSET_ADDR_IN_EEPROM + i);
        // EEPROM needs 'colddown' after IO
        EEPROM_IO_DELAY();
        if (!ret) {
            break;
        }
    }

    osDelay(10);
    // read them
    uint8_t read_back[PSN_BYTE_COUNT] = { 0 };
    for (int i = 0; i < PSN_BYTE_COUNT; i++) {
        ret = ret && eeprom_byte_read(read_back + i, PSN_OFFSET_ADDR_IN_EEPROM + i);
        // EEPROM needs 'colddown' after IO
        EEPROM_IO_DELAY();
        if (!ret) {
            break;
        }
    }

    // compare read and written, they should be equal
    ret = ret && memcmp(read_back, psn_byte_array, PSN_BYTE_COUNT) == 0;
    *return_value = ret;
}
