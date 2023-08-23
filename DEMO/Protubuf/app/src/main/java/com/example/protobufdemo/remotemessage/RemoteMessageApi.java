package com.example.protobufdemo.remotemessage;

import static com.example.protobufdemo.remotemessage.RemoteMessageConstants.TO_MCU_LOGIN_TAG;
import static com.example.protobufdemo.remotemessage.RemoteMessageConstants.TO_MCU_LOGOUT_TAG;

import android.util.Log;

import com.example.protobufdemo.RemoteMessage;

public class RemoteMessageApi {
    private String TAG = "RemoteMessageApi";

    RemoteMessage.to_mcu.Builder mcuInfo ;

    /**
     * 调用jni发送报文给mcu
     * @param req
     */

    private RemoteMessage.remote_call_err_code sendRemoteRequestToMcu(RemoteMessage.to_mcu req){
        //将req对象序列化为byte报文发给mcu,暂时未调用jni发报文方法
        Log.d(TAG,req.toString());
        return RemoteMessage.remote_call_err_code.no_impl;
    }


    /**
     * login
     * @param req
     * @return
     */
    private RemoteMessage.remote_call_err_code remoteCllServiceForLogin(RemoteMessage.login_req req){
        mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setSeq(1);
        mcuInfo.setLogin(req);
        //调用to_mcu方法发送指令
        return sendRemoteRequestToMcu(mcuInfo.build());
    }

    /**
     * logout
     * @param req
     * @return
     */
    private RemoteMessage.remote_call_err_code remoteCllServiceForLogout(RemoteMessage.logout_req req){
        mcuInfo = RemoteMessage.to_mcu.newBuilder();
        mcuInfo.setSeq(2);
        mcuInfo.setLogout(req);
        //调用to_mcu方法发送指令
        return sendRemoteRequestToMcu(mcuInfo.build());
    }


    public RemoteMessage.remote_call_err_code remoteCllService(Object obj){
        RemoteMessage.remote_call_err_code err_code;
        Log.d(TAG,"obj.getClass() = " + obj.getClass().getSimpleName());
        switch (obj.getClass().getSimpleName()){
            case TO_MCU_LOGIN_TAG:
                err_code = remoteCllServiceForLogin((RemoteMessage.login_req) obj);
                break;
            case TO_MCU_LOGOUT_TAG:
                err_code = remoteCllServiceForLogout((RemoteMessage.logout_req) obj);
                break;
            default:
                err_code = RemoteMessage.remote_call_err_code.req_type_err;
                break;
        }
        return err_code;
    }
}


