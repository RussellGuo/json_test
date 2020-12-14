/*
 * service_factory_test_hw.c
 *
 * hardware-depended part of factory-test service for the SAIP
 *
 *  Created on: Dec 14, 2020
 *      Author: Guo Qiang
 */

#if defined(SAIP_BOARD)

#include "semantic_api.h"

#include "cmsis_os2.h"
#include "gd32e10x.h"

// ReplyToStartFactoryTest, the function is in the API layer of the host-MCU communication
// protocol and runs in the protocol receiving thread. It will be called by protocol receiving thread
// (after checking the validity of the parameters), then do all the test, and store the result
// and then the receiving thread will send them to the host as a respose. Because the factory test is executed
// under specific circumstances, it does not need to be placed in a separate thread, but directly
// in the receiving thread. In this case, it is not a problem that the receiving thread is
// blocked for tens of hundreds of milliseconds. See the documentation for details.
// parameters:
//   [out] error_code, the ptr to an error code, which the value will be sent to the host
//   [out] test_item_list, the test result 0/1 (false/true) array for echo test item.
//   [in]  seq, the request seq
void ReplyToStartFactoryTest(res_error_code_t *error_code, uint32_t *test_item_list, serial_datagram_item_t seq)
{
    (void)seq;
   *error_code = NO_ERROR;
    bool value = true;

    // TODO: Place the real code. Currently, it simply returns success and is used to debug the framework.
    for (int i = 0; i < FACTORY_TEST_RESULT_COUNT; i++) {
        test_item_list[i] = value;
    }
}

#endif
