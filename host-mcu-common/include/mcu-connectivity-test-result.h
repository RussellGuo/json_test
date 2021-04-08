/*
 * mcu-connectivity-test-result.h
 *
 * definition of connectivity test mode and mode parameter. those value will be used in the
 * protocol between the host and the MCU
 *
 *  Created on: Dec 14, 2020
 *      Author: Guo Qiang
 *   please reference the document "SaIP communication protocol between the host and the MCU"
 */


#ifndef __MCU_CONNECTIVITY_TEST_RESULT_H_
#define __MCU_CONNECTIVITY_TEST_RESULT_H_

#ifdef __cplusplus
extern "C" {
#endif

// The index of the connectivity test result array. Currently 6 are defined and 4 are reserved
typedef enum {
    CONTIVITY_TEST_RESULT_IDX_1             = 0,
    CONTIVITY_TEST_RESULT_IDX_2             = 1,
    CONTIVITY_TEST_RESULT_IDX_3             = 2,
    CONTIVITY_TEST_RESULT_IDX_T_SENSOR      = 3,
    CONTIVITY_TEST_RESULT_IDX_MAX           = 9,
} connectivity_test_result_idx_t;

#ifdef __cplusplus
}
#endif

#endif //__MCU_CONNECTIVITY_TEST_RESULT_H_
