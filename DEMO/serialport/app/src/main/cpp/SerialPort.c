//
// Created by ubuntu on 2023/8/22.
//
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "../host/pb_decode.h"
#include "../host/pb_encode.h"
#include "../host/remote_message.pb.h"
#include "../host/uart_io_api.h"
#include "../host/serial_datagram.h"
#include "ALog.h"

/* Header for class com_huaqin_serialport_SerialPort */

#ifndef _Included_com_huaqin_serialport_SerialPort
#define _Included_com_huaqin_serialport_SerialPort

#ifdef __cplusplus
extern "C" {
#endif
JNIEnv *java_env;

//在此处回调到java层
void java_call_back(const void *data_ptr) {
    jclass jSdkClass =(*java_env)->FindClass(java_env,"com/huaqin/serialport/SerialPort");
    if (jSdkClass == 0) {
        LOGD("Unable to find class");
        return;
    }
    //找到需要调用的方法ID
    jmethodID javaCallback = (*java_env)->GetMethodID(java_env, jSdkClass,
                                                 "onNativeCallBack", "(I)V");
    //这时候要回调还没有jobject，那就new 一个
    jmethodID sdkInit = (*java_env)->GetMethodID(java_env, jSdkClass,"<init>","()V");
    jobject jSdkObject = (*java_env)->NewObject(java_env,jSdkClass,sdkInit);
    //进行回调
    (*java_env)->CallVoidMethod(java_env, jSdkObject, javaCallback,data_ptr);
}




/*
 * Class:     com_huaqin_serialport_SerialPort
 * Method:    initUart
 * Signature: ()Z
 */

JNIEXPORT jboolean JNICALL Java_com_huaqin_serialport_SerialPort_initUart
        (JNIEnv *, jobject){
    bool ret;
    ret = init_uart_io_api();
    return ret;
}

/*
 * Class:     com_huaqin_serialport_SerialPort
 * Method:    serialDatagramSend
 * Signature: ()Z
 */
JNIEXPORT jboolean JNICALL Java_com_huaqin_serialport_SerialPort_serialDatagramSend
        (JNIEnv *, jclass){
    bool bet;
    login_req login_req_obj = {"caomeng", "123456"};
    //login_req_obj.username = "caomeng";
    //login_req_obj.password = "123456";

    to_mcu to_mcu_obj = to_mcu_init_zero;
    to_mcu_obj.seq = 1;
    to_mcu_obj.which_req = to_mcu_login_tag;
    to_mcu_obj.req.login = login_req_obj;

    unsigned char login_req_buf[600] = {0};

    pb_ostream_t out_stream = pb_ostream_from_buffer(login_req_buf, sizeof(login_req_buf));

    bool status = pb_encode(&out_stream, to_mcu_fields, &to_mcu_obj);
    printf("outstream:%x %x %x %x %x %x %x %x %x %x %x %x %x %x\n", login_req_buf[0], login_req_buf[1], login_req_buf[2], login_req_buf[3], login_req_buf[4], login_req_buf[5],
           login_req_buf[6], login_req_buf[7], login_req_buf[8], login_req_buf[9], login_req_buf[10], login_req_buf[11], login_req_buf[12], login_req_buf[13]);

    bet = send_datagram(login_req_buf, out_stream.bytes_written);
    return bet;
}


/*
 * Class:     com_huaqin_serialport_SerialPort
 * Method:    serialDatagramReceiveLoop
 * Signature: ()Z
 */
void JNICALL Java_com_huaqin_serialport_SerialPort_serialDatagramReceiveLoop
        (JNIEnv *env, jobject object){
    LOGD("Java_com_huaqin_serialport_SerialPort_serialDatagramReceiveLoop");
    java_env = env;
    serial_datagram_receive_loop(0);

}

void receive_loop_result(const void *data_ptr, unsigned short len){
    LOGD(" result = %d" ,data_ptr);
    java_call_back(data_ptr);
}

#ifdef __cplusplus
}
#endif
#endif
