package com.huaqin.posservices.remotemessage;

import android.os.RemoteCallbackList;
import android.util.Log;
import com.google.protobuf.InvalidProtocolBufferException;
import com.huaqin.posservices.IPosCallback;
import com.huaqin.posservices.PosService;
import com.huaqin.posservices.PosServiceManagerService;
import com.huaqin.posservices.RemoteMessage;
import com.huaqin.posservices.SerialPort;


public class RemoteMessageApi {
    private String TAG = "RemoteMessageApi";

    /**
     * login
     * @param req
     * @return
     */
    public Boolean remoteCallService(RemoteMessage.login_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setLogin(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte,toMcuByte.length);
    }

    /**
     * logout
     * @param req
     * @return
     */
    public Boolean remoteCallService(RemoteMessage.logout_req req){
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setLogout(req);
        byte[] toMcuByte = mcuInfo.build().toByteArray();
        //调用jni方法给mcu发送指令
        return SerialPort.serialDatagramSend(toMcuByte,toMcuByte.length);
    }

    /**
     * 接收mcu报文
     * @param obj
     * @return
     */
    public void remoteCallbackService(byte[] obj){
        RemoteMessage.from_mcu forMcu = forMcuUnpack(obj);
        RemoteCallbackList<IPosCallback> mCallbacks = PosServiceManagerService.init().getPosService().getmCallbacks();
        final int N = mCallbacks.beginBroadcast();
        for (int i = 0; i < N; i++) {
            if(forMcu != null){
                try {
                Log.d(TAG,"forMcu.getResCase() = " + forMcu.getResCase());
                switch (forMcu.getResCase()){
                    case LOGIN:
                        mCallbacks.getBroadcastItem(i).onLoginRes(obj);
                        break;
                    case LOGOUT:
                        mCallbacks.getBroadcastItem(i).onLogoutRes(obj);
                        break;
                    case GET_VERSION_INFO:
                        mCallbacks.getBroadcastItem(i).onVersionInfoRes(obj);
                        break;
                    case KEY:
                        mCallbacks.getBroadcastItem(i).onKeyRes(obj);
                        break;
                    case LOG:
                        mCallbacks.getBroadcastItem(i).onLogRes(obj);
                        break;
                    default:
                        mCallbacks.getBroadcastItem(i).onErrorRes(obj);
                        break;
                    }
                } catch (Exception e){
                    e.printStackTrace();
                }
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


