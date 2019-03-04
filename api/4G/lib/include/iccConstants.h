#ifndef _ICC_CONSTANTS_H
#define _ICC_CONSTANTS_H

//EF Path
#define MF_SIM "3F00"

// size of GET_RESPONSE for EF's
#define GET_RESPONSE_EF_SIZE_BYTES   15

//command for sim access
typedef enum{
    COMMAND_GET_RESPONSE = 0xc0,
    COMMAND_MAX
}EF_COMMAND_TYPE;

//fild id for EF's
typedef enum{
    EF_ICCID = 0x2fe2,
    EF_MAX
}EF_FILE_ID;

typedef struct {
    int sw1;
    int sw2;
    char *simResponse;  /* In hex string format ([a-fA-F0-9]*), except for SIM_AUTHENTICATION
                           response for which it is in Base64 format, see 3GPP TS 31.102 7.1.2 */
} RIL_SIM_IO_Response;
#endif

