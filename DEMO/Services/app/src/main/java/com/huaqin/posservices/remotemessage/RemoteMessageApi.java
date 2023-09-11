package com.huaqin.posservices.remotemessage;


import static com.huaqin.posservices.remotemessage.RemoteMessageConstants.READ_CARD_CALL_BACK_TAG;
import static com.huaqin.posservices.remotemessage.RemoteMessageConstants.TO_MCU_LOGIN_TAG;
import static com.huaqin.posservices.remotemessage.RemoteMessageConstants.TO_MCU_LOGOUT_TAG;

import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.util.Log;
import com.example.protobufdemo.RemoteMessage;
import com.huaqin.posservices.IReadCardCallback;
import com.huaqin.posservices.PosCardService;
import java.util.HashMap;

public class RemoteMessageApi {
    private String TAG = "RemoteMessageApi";
    ///private HashMap<IBinder, ServiceBinderListener> mListenersMap;
    RemoteMessage.to_mcu.Builder mcuInfo ;
    private HashMap<IBinder, Object> mListenersMap;
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


    public RemoteMessage.remote_call_err_code remoteCllService(Object obj, HashMap<IBinder, Object> callBackList){
        RemoteMessage.remote_call_err_code err_code;
        mListenersMap = callBackList;
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

    public void fotMCUServices (byte[] obj, RemoteCallbackList<IReadCardCallback> callback){
        PosCardService posservice = new PosCardService();
        posservice.startCallback(callback);
        /*if(obj != null && obj.length > 0){
            try {
                RemoteMessage.from_mcu forMcu = RemoteMessage.from_mcu.parseFrom(obj);
                Set set = mListenersMap.entrySet();
                Iterator i = set.iterator();

                while (i.hasNext()) {
                    Map.Entry entry = (Map.Entry)i.next();
                    switch (entry.getValue().getClass().getTypeName()){
                        case  READ_CARD_CALL_BACK_TAG :
                            PosCardService posservice = new PosCardService();
                            posservice.startCallback();
                            break;
                        default:
                            break;
                    }
                }
            } catch (InvalidProtocolBufferException e) {
                throw new RuntimeException(e);
            }
        }*/


    }


}


