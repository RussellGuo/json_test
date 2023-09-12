package com.huaqin.posservices.remotemessage;


import static com.huaqin.posservices.remotemessage.RemoteMessageConstants.READ_CARD_CALL_BACK_TAG;
import static com.huaqin.posservices.remotemessage.RemoteMessageConstants.TO_MCU_LOGIN_TAG;
import static com.huaqin.posservices.remotemessage.RemoteMessageConstants.TO_MCU_LOGOUT_TAG;

import android.os.RemoteCallbackList;
import android.util.Log;
import com.example.protobufdemo.RemoteMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import com.huaqin.posservices.IReadCardCallback;
import com.huaqin.posservices.PosCardService;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

public class RemoteMessageApi {
    private String TAG = "RemoteMessageApi";
    RemoteMessage.to_mcu.Builder mcuInfo ;
    private HashMap<String, Object> mListenersMap;
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

    /**
     * 发送报文总入口
     * @param obj
     * @param callBackList
     * @return
     */
    public RemoteMessage.remote_call_err_code remoteCllService(Object obj, HashMap<String, Object> callBackList){
        RemoteMessage.remote_call_err_code err_code;
        mListenersMap = callBackList;
        byte[]  to_mcu_buf = {0x08, 0x02, 0x10, 0x04, 0x1a, 0x02, 0x08, 0x02};
        Log.d(TAG,"obj.getClass() = " + obj.getClass().getSimpleName());
        forMcuServices(to_mcu_buf);
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

    /**
     * 接收来自jni的报文总入口
     * @param obj
     */
    public void forMcuServices (byte[] obj){
        RemoteMessage.from_mcu forMcu = forMcuUnpack(obj);
        if(obj != null && obj.length > 0){
            Set set = mListenersMap.entrySet();
            Iterator i = set.iterator();
            while (i.hasNext()) {
                Map.Entry entry = (Map.Entry)i.next();
                switch (entry.getKey().toString()){//根据注册回调启动回调函数
                    case  READ_CARD_CALL_BACK_TAG :
                        //启动回调
                        new PosCardService().startCallback((RemoteCallbackList<IReadCardCallback>) entry.getValue(), forMcu);
                        break;
                    default:
                        break;
                }
            }
        }
    }
}


