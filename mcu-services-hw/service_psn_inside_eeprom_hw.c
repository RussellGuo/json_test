/*
 * service_psn_inside_eeprom_hw.c
 *
 * Need to save PSN to EEPROM on production line.
 *
 * hardware-depended part of 'save PSN' and 'get PSN' service for the SAIP
 *
 *  Created  on: Dec 14, 2020
 *  Modified on: Mar 10, 2021 for 'get PSN'
 *      Author: Guo Qiang
 */

#include "semantic_api.h"
#include "gd32e10x.h"
#include "cmsis_os2.h"

#include "eeprom.h"

#include "db9_init_for_factory.h"

#include <string.h>

// PSN will save here according the EEPROM layout design from people of product line
#define PSN_OFFSET_ADDR_IN_EEPROM 4
#define EEPROM_IO_DELAY() osDelay(2)

// ReplyToSavePsnIntoEeprom, the function is in the API layer of the host-MCU communication
// protocol and runs in the protocol receiving thread. It will be called by protocol receiving thread
// (after checking the validity of the parameters), then do all the test, and store the result
// and then the receiving thread will send them to the host as a respose. Because the saving psn is executed
// under specific circumstances, it does not need to be placed in a separate thread, but directly
// in the receiving thread. In this case, it is not a problem that the receiving thread is
// blocked for hundreds of milliseconds. See the documentation for details.
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

    db9_init_for_factory();

}

// ReplyToGetPsnFromEeprom, the function is in the API layer of the host-MCU communication
// protocol and runs in the protocol receiving thread. It will be called by protocol receiving thread
// (after checking the validity of the parameters), then do all the test, and store the result
// and then the receiving thread will send them to the host as a respose. Because the saving psn is executed
// under specific circumstances, it does not need to be placed in a separate thread, but directly
// in the receiving thread. In this case, it is not a problem that the receiving thread is
// blocked for hundreds of milliseconds. See the documentation for details.
// parameters:
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [out] psn_byte_array, the byte array of PSN with length PSN_BYTE_COUNT
//   [in]  seq, the request seq
void ReplyToGetPsnFromEeprom(res_error_code_t *error_code, uint8_t *psn_byte_array, serial_datagram_item_t seq)
{
    *error_code = NO_ERROR;
    (void)seq;

    bool ret = true;

    osDelay(10);
    // read them
    for (int i = 0; i < PSN_BYTE_COUNT; i++) {
        ret = ret && eeprom_byte_read(psn_byte_array + i, PSN_OFFSET_ADDR_IN_EEPROM + i);
        // EEPROM needs 'colddown' after IO
        EEPROM_IO_DELAY();
        if (!ret) {
            break;
        }
    }

    if (!ret) {
        memset(psn_byte_array, 0, PSN_BYTE_COUNT);
    }

    db9_init_for_factory();
}
