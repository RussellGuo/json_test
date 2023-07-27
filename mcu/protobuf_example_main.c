#include <assert.h>

// 在MCU端用nonopb编解码的示例，郭强，2023-7-25

// 这俩是固定用法
#include "pb_decode.h"
#include "pb_encode.h"
// 这是根据remote_message.proto 和 .options 生成的头文件
#include "remote_message.pb.h"

int protobuf_example_main(void) {
    // 对req_buf解码。这组数据是我手工复制过来的，从C++的编码输出
    const unsigned char req_buf[] = {0x0a, 0x07, 0x52, 0x75, 0x73, 0x73, 0x65, 0x6c, 0x6c, 0x12, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35};
    // 这个解码辅助steam较为适合我们使用，因为MCU获得数据后去解码，是一段二进制数据。而且我们不光要解码，还要知道解码是否“消耗了”全部输入。如果不是代表出问题了。
    pb_istream_t in_stream = pb_istream_from_buffer(req_buf, sizeof req_buf);
    // 存放解码结果的，先清空
    login_req req = login_req_init_zero;
    // 正式解码
    bool status = pb_decode(&in_stream, login_req_fields, &req);

    // 用assert来观察输出。需要用debug模式来运行本程序，因为release版会忽略assert
    assert(status);
    assert(strcmp(req.username, "Russell") == 0);
    assert(strcmp(req.password, "12345") == 0);
    assert(in_stream.bytes_left == 0);

    // nanopb的编码样例程序
    // 准备数据
    login_res res = {.status = boolean_t_unknown};
    // 准备buf
    unsigned char res_buf[600] = {0};
    // 准备输出stream，其实就是输出到buf中
    pb_ostream_t out_stream = pb_ostream_from_buffer(res_buf, sizeof res_buf);
    // 编码
    status = pb_encode(&out_stream, login_res_fields, &res);
    // 检查返回值，同上面的一组assert
    assert(status);
    // 运行观察过，输出结果是 {8 2} 序列。C++的解码程序也被证明是认得的。
    assert(out_stream.bytes_written == 2);

    assert(res_buf[0] == 8 && res_buf[1] == 2);

    return !status;
}
