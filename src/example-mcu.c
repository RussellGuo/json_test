#include <assert.h>
#include <stdio.h>

#include "pb_decode.h"
#include "pb_encode.h"
#include "remote_message.pb.h"

int main() {
    login_req req = login_req_init_zero;
    const unsigned char req_buf[] = {0x0a, 0x07, 0x52, 0x75, 0x73, 0x73, 0x65, 0x6c, 0x6c, 0x12, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35};
    pb_istream_t in_stream = pb_istream_from_buffer(req_buf, sizeof req_buf);
    bool status = pb_decode(&in_stream, login_req_fields, &req);
    assert(status);
    assert(strcmp(req.username, "Russell") == 0);
    assert(strcmp(req.password, "12345") == 0);
    assert(in_stream.bytes_left == 0);

    login_res res = {.status = boolean_t_unknown};
    unsigned char res_buf[600] = {0};
    pb_ostream_t out_stream = pb_ostream_from_buffer(res_buf, sizeof res_buf);
    status = pb_encode(&out_stream, login_res_fields, &res);
    assert(status);
    assert(out_stream.bytes_written == 2);
    assert(res_buf[0] == 8 && res_buf[1] == 2);

    return !status;
}
