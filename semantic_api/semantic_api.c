
#include "semantic_api.h"
#include <stdio.h>
#include "datagram_codec.h"

#define MAX_RESPONSE_SIZE 80

bool send_remote_res(from_mcu *from_mcu_obj) {
    unsigned char from_mcu_buf[MAX_RESPONSE_SIZE] = {0};
    pb_ostream_t out_stream = pb_ostream_from_buffer(from_mcu_buf, sizeof from_mcu_buf);
    bool status = pb_encode(&out_stream, from_mcu_fields, from_mcu_obj);
    if (status) {
        status = send_datagram(from_mcu_buf, out_stream.bytes_written);
    }
    return status;
}

// 处理来自host的报文
bool process_incoming_datagram(const void *data_ptr, unsigned short len) {
    pb_istream_t in_stream = pb_istream_from_buffer(data_ptr, len);                       // 准备解码
    to_mcu to_mcu_obj = to_mcu_init_zero;                                                 // 解码结果的对象
    bool status = pb_decode(&in_stream, to_mcu_fields, &to_mcu_obj);                      // 解！
    if (status) {                                                                         // 解码成功，准备响应报文
        from_mcu from_mcu_obj = from_mcu_init_zero;                                       // 解码报文对象准备
        remote_call_err_code err_code = remote_call_service(&to_mcu_obj, &from_mcu_obj);  // 调用服务分发总程序
        // TODO: 检查 in_stream.bytes_left
        from_mcu_obj.seq = to_mcu_obj.seq;                                                // 序号和请求的一致
        from_mcu_obj.err_code = err_code;                                                 // 错误码存放
        status = send_remote_res(&from_mcu_obj);                                          // 结果发送回去
    }

    // TODO: 统计编解码的个数等事宜

    return status;
}

remote_call_err_code remote_call_service_for_psam_slot(const psam_slot_req *psam_slot_req_obj, psam_slot_res *psam_slot_res_obj) {
    (void)psam_slot_req_obj;
    (void)psam_slot_res_obj;
    psam_slot_res_obj->res_code = 2;
    return remote_call_err_code_no_impl;
}

bool test_encoding(void)
{
    from_mcu from_mcu_obj = from_mcu_init_zero;
    from_mcu_obj.seq = 1;
    from_mcu_obj.err_code = remote_call_err_code_no_impl;
    from_mcu_obj.which_res = from_mcu_psam_slot_tag;
    remote_call_service_for_psam_slot(0, &from_mcu_obj.res.psam_slot);
    unsigned char from_mcu_buf[MAX_RESPONSE_SIZE] = {0};
    pb_ostream_t out_stream = pb_ostream_from_buffer(from_mcu_buf, sizeof from_mcu_buf);
    bool status = pb_encode(&out_stream, from_mcu_fields, &from_mcu_obj);
    return status;
}
