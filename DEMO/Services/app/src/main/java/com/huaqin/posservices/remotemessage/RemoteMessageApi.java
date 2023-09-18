package com.huaqin.posservices.remotemessage;


import android.util.Log;
import com.example.protobufdemo.RemoteMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import com.huaqin.posservices.SerialPort;

import java.util.HashMap;

public class RemoteMessageApi {
    private String TAG = "RemoteMessageApi";
    RemoteMessage.to_mcu.Builder mcuInfo ;

    /**
     * login
     * @param req
     * @return
     */
    private Boolean remoteCllService(RemoteMessage.login_req req){
        mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setSeq(1);
        mcuInfo.setLogin(req);
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(mcuInfo.build().toByteArray());
    }

    /**
     * logout
     * @param req
     * @return
     */
    private Boolean remoteCllService(RemoteMessage.logout_req req){
        mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setSeq(2);
        mcuInfo.setLogout(req);
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(mcuInfo.build().toByteArray());
    }

    /**
     * 接收mcu报文
     * @param obj
     * @return
     */
    public void remoteCllService(byte[] obj){
        RemoteMessage.from_mcu forMcu = forMcuUnpack(obj);
        if(forMcu != null){
            String resCase = forMcu.getResCase().toString();
            Log.d(TAG,"forMcu.getResCase() = " + resCase);
            switch (resCase){
                case RemoteMessageConstants.FORM_MCU_LOGIN:
                    break;
                case RemoteMessageConstants.FORM_MCU_LOGOUT:
                    break;
                case RemoteMessageConstants.FORM_MCU_GET_VERSION_INFO:
                    break;
                case RemoteMessageConstants.FORM_MCU_KEY:
                    break;
                case RemoteMessageConstants.FORM_MCU_LOG:
                    break;
                default:
                    break;
            }
        }
    }

    /**
     * 解码mcu报文
     * @param obj
     * @return
     */
    private RemoteMessage.from_mcu forMcuUnpack(byte[] obj) {
        RemoteMessage.from_mcu forMcu;
        try {
            forMcu = RemoteMessage.from_mcu.parseFrom(obj);
        } catch (InvalidProtocolBufferException e) {
            throw new RuntimeException(e);
        }
        return forMcu;
    }

}


