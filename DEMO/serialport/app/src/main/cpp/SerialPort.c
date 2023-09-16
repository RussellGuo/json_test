//
// Created by ubuntu on 2023/8/22.
//
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "remote_message.pb.h"
#include "datagram_codec.h"
#include "ALog.h"
#include "uart_io_api.h"
#include "semantic_api.h"

/* Header for class com_huaqin_serialport_SerialPort */

#ifndef _Included_com_huaqin_serialport_SerialPort
#define _Included_com_huaqin_serialport_SerialPort

#ifdef __cplusplus
extern "C" {
#endif
JNIEnv *java_env;

//在此处回调到java层
void java_call_back(const void *data_ptr, int len) {
    unsigned char *buf = data_ptr;
    int i;
    jcharArray array = (*java_env)->NewCharArray(java_env, len);//定义数据变量
    jchar *pArray ;

    if(array == NULL){
        LOGE("receive_callback: NewBooleanArray error.");
        return;
    }
    pArray = (jchar *)calloc(len, sizeof(jchar));//开辟jchar类型内存空间
    if(pArray == NULL){
        LOGE("receive_callback: calloc error.");
        return;
    }

    for(i = 0; i < len; i++) {
        *(pArray + i) = *(buf + i);//复制buf数据元素到pArray内存空间
    }
    //copy buffer to jcharArray
    (*java_env)->SetCharArrayRegion(java_env, array, 0, len, pArray);//复制pArray的jchar数据元素到jcharArray
    //invoke java callback method
    jclass jSdkClass =(*java_env)->FindClass(java_env,"com/huaqin/posservices/SerialPort");
    if (jSdkClass == 0) {
        LOGD("Unable to find class");
        return;
    }
    //找到需要调用的方法ID
    jmethodID javaCallback = (*java_env)->GetMethodID(java_env, jSdkClass, "onNativeCallBack", "([CI)V");
    jmethodID sdkInit = (*java_env)->GetMethodID(java_env, jSdkClass,"<init>","()V");
    jobject jSdkObject = (*java_env)->NewObject(java_env,jSdkClass,sdkInit);
    //进行回调
    (*java_env)->CallVoidMethod(java_env, jSdkObject, javaCallback, array, len);
    //release resource
    (*java_env)->DeleteLocalRef(java_env,array);
    free(pArray);
    pArray = NULL;
}

/*
 * Class:     com_huaqin_serialport_SerialPort
 * Method:    initUart
 * Signature: ()Z
 */

JNIEXPORT jboolean JNICALL Java_com_huaqin_posservices_SerialPort_initUart
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
JNIEXPORT jboolean JNICALL Java_com_huaqin_posservices_SerialPort_serialDatagramSend
        (JNIEnv *env, jclass, jbyteArray data){
    bool bet;
    LOGD("outstream:%x\n",data );
    //将传入的jbyteArray 转换为 c 中的数据
    jbyte* bytekey =(*env)->GetByteArrayElements(env,data, 0); //获取数组指针
    int bytekeylen = (*env)->GetArrayLength(env,data);//获取数组长度

    LOGD("bytekeylen:%d" ,bytekeylen );
    //实例，返回数组bytekey
    jbyteArray  jarrRV =(*env)->NewByteArray(env,bytekeylen);
    (*env)->SetByteArrayRegion(env,jarrRV, 0,bytekeylen,bytekey);
    unsigned char* byteData[600];
    (*env)->GetByteArrayRegion(env,jarrRV,0,bytekeylen,byteData);
    bet = send_datagram(byteData, bytekeylen);
    return bet;
}

/*
 * Class:     com_huaqin_serialport_SerialPort
 * Method:    serialDatagramReceiveLoop
 * Signature: ()Z
 */
void JNICALL Java_com_huaqin_posservices_SerialPort_serialDatagramReceiveLoop
        (JNIEnv *env, jobject object){
    LOGD("Java_serialDatagramReceiveLoop");
    java_env = env;
    serial_datagram_receive_loop(0);

}

// 处理来自host的报文
bool process_incoming_datagram(const void *data_ptr, unsigned short len) {
    LOGD("process_incoming_datagram");
    const uint8_t *buf = data_ptr;
    for (int i = 0; i < len; i++) {
        LOGD("buf[%d]= %x ", i, buf[i]);
    }

    pb_istream_t in_stream = pb_istream_from_buffer(data_ptr, len);                       // 准备解码

    from_mcu from_mcu_obj = from_mcu_init_zero;                                                 // 解码结果的对象
    bool status = pb_decode(&in_stream, from_mcu_fields, &from_mcu_obj);                      // 解！
    LOGD("pb_decode status:%d", status);
    if (status) {
        LOGD("err_code:%d",from_mcu_obj.err_code);
        LOGD("status:%d",from_mcu_obj.res.login.status);
    }
    // TODO: 统计编解码的个数等事宜
    java_call_back(data_ptr, len);
    return status;
}

#ifdef __cplusplus
}
#endif
#endif
