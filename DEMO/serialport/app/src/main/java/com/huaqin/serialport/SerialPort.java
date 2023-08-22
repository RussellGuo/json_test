package com.huaqin.serialport;

public class SerialPort {
    //加载库文件
    static {
        System.loadLibrary("serialport");
    }

    //初始化串口
    public static native boolean initUart();
    //发送指令
    public static native void serialDatagramSend();
    //获取结果
    public static native void serialDatagramReceiveLoop();
}
