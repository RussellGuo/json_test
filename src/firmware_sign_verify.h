#ifndef __FIRMWARE_SIGN_VERIFY_H__
#define __FIRMWARE_SIGN_VERIFY_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

bool firmware_sign_verify(const uint8_t *firmware_memory, const uint8_t *public_key_pem_string);

#endif
