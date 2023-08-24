package com.huaqin.serialport;

import android.util.Log;

public class SerialPort {
    //加载库文件
    static {
        System.loadLibrary("serialport");
    }

    //供JNI端回调的Java方法
    public void onNativeCallBack(int count) {
        Log.e( "zhangwencai", "onNativeCallBack: " + count);
    }

    //初始化串口
    public static native boolean initUart();
    //发送指令
    public static native boolean serialDatagramSend();
    //获取结果
    public static native void serialDatagramReceiveLoop();
}
