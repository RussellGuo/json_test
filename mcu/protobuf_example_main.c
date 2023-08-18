#include <stdio.h>

// 在MCU端用nonopb编解码的示例，郭强，2023-7-25
// 在MCU端处理较复杂的protobuf的例子 郭强，2023-8-18

// 这俩是固定用法
#include "pb_decode.h"
#include "pb_encode.h"
// 这是根据remote_message.proto 和 .options 生成的头文件
#include "remote_message.pb.h"

int protobuf_example_main(void) {
    // 对login_req_buf解码。这组数据是我手工复制过来的，从C++的编码输出
    const unsigned char login_req_buf[] = {0x0a, 0x07, 0x52, 0x75, 0x73, 0x73, 0x65, 0x6c, 0x6c, 0x12, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35};
    // 这个解码辅助steam较为适合我们使用，因为MCU获得数据后去解码，是一段二进制数据。而且我们不光要解码，还要知道解码是否“消耗了”全部输入。如果不是代表出问题了。
    pb_istream_t in_stream = pb_istream_from_buffer(login_req_buf, sizeof login_req_buf);
    // 存放解码结果的，先清空
    login_req login_req_obj = login_req_init_zero;
    // 正式解码
    bool status = pb_decode(&in_stream, login_req_fields, &login_req_obj);

    printf("login_req_obj decoding status: %d\n", status);
    printf("login_req_obj login username: %s\n",login_req_obj.username);
    printf("login_req_obj login password: %s\n",login_req_obj.password);
    printf("login_req_buf rest byte(s): %d\n", in_stream.bytes_left);

    // nanopb的编码样例程序
    // 准备数据
    login_res login_res_obj = {.status = boolean_t_unknown};
    // 准备buf
    unsigned char login_res_buf[600] = {0};
    // 准备输出stream，其实就是输出到buf中
    pb_ostream_t out_stream = pb_ostream_from_buffer(login_res_buf, sizeof login_res_buf);
    // 编码
    status = pb_encode(&out_stream, login_res_fields, &login_res_obj);
    // 检查返回值
    printf("login_res_obj encoding status: %d\n", status);
    // 运行观察过，输出结果是 {8 2} 序列。C++的解码程序也被证明是认得的。
    printf("login_res_obj encoding-bytes: %d\n", out_stream.bytes_written);
    printf("login_res_buf byte: [%02X %02X]\n", login_res_buf[0], login_res_buf[1]);

    // 来个复杂的解码例子：to_mcu. 这段数据也是复制来的。
    const unsigned char to_mcu_buf[] = {0x08, 0x01, 0x15, 0xff, 0xff, 0x00, 0x00, 0x1a, 0x10, 0x0a, 0x07, 0x52, 0x75, 0x73, 0x73, 0x65, 0x6c, 0x6c, 0x12, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35};
    in_stream = pb_istream_from_buffer(to_mcu_buf, sizeof to_mcu_buf);
    to_mcu to_mcu_obj = to_mcu_init_zero;
    status = pb_decode(&in_stream, to_mcu_fields, &to_mcu_obj);

    printf("to_mcu_obj decoding status: %d\n", status);
    printf("to_mcu_obj login username: %s\n",to_mcu_obj.req.login.username);
    printf("to_mcu_obj login password: %s\n",to_mcu_obj.req.login.password);
    printf("to_mcu_buf rest byte(s): %d\n", in_stream.bytes_left);


    // 构造一个编码的例子
    from_mcu from_mcu_obj = from_mcu_init_zero;
    from_mcu_obj.crc = 0xFFFEU;
    from_mcu_obj.seq = 2U;
    from_mcu_obj.which_res = from_mcu_login_tag;
    from_mcu_obj.res.login = login_res_obj;
    unsigned char from_mcu_buf[600] = {0};
    out_stream = pb_ostream_from_buffer(from_mcu_buf, sizeof from_mcu_buf);
    status = pb_encode(&out_stream, from_mcu_fields, &from_mcu_obj);
    // 预期是这一串，我纯手敲的（debug方式运行到这里然后观察变量值，再抄下来）。这段会放在C++里解码
    const unsigned char expected_from_mcu_binary[] = {0x08, 0x02, 0x15, 0xfe, 0xff, 0x00, 0x00, 0x1a, 0x02, 0x08, 0x02};
    printf("from_mcu_obj encoding status: %d\n", status);
    bool is_ok = out_stream.bytes_written == sizeof expected_from_mcu_binary;
    is_ok &= memcmp(from_mcu_buf, expected_from_mcu_binary, sizeof expected_from_mcu_binary) == 0;
    printf("from_mcu_obj encoding data-matching: %d\n", is_ok);

    return !status;
}
