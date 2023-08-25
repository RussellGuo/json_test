//
// Created by ubuntu on 2023/8/25.
//

#include "remote_message.pb.h"
#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

PB_BIND(login_req, login_req, AUTO)


PB_BIND(login_res, login_res, AUTO)


PB_BIND(logout_req, logout_req, AUTO)


PB_BIND(logout_res, logout_res, AUTO)


PB_BIND(key_evt, key_evt, AUTO)


PB_BIND(log_evt, log_evt, AUTO)


PB_BIND(to_mcu, to_mcu, AUTO)


PB_BIND(from_mcu, from_mcu, 2)

