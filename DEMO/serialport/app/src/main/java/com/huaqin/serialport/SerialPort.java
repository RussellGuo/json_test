package com.huaqin.serialport;

import android.util.Log;

public class SerialPort {
    //加载库文件
    static {
        System.loadLibrary("serialport");
    }
    private final String TAG = "SerialPort";
    //回调到各个线程
    public interface OnSubProgressListener {
        public int onProgressChange(long total, long already);
    };

    //供JNI端回调的Java方法
    public void onNativeCallBack(int obj) {
        Log.e( TAG, "onNativeCallBack: " + obj);

    }

    //初始化串口
    public static native boolean initUart();
    //发送指令
    public static native boolean serialDatagramSend();
    //获取结果
    public static native void serialDatagramReceiveLoop();
}
