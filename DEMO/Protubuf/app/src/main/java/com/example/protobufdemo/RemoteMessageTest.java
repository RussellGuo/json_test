package com.example.protobufdemo;


import static com.example.protobufdemo.RemoteMessage.boolean_t.unknown;

import android.util.Log;

import com.google.protobuf.InvalidProtocolBufferException;

public class RemoteMessageTest {


    private String TAG = "RemoteMessageTest";


    private RemoteMessage.to_mcu toMcu(byte[] toMcuBuf){
        /*创建编码数据
        toMcu = seq: 1
        login {
            username: "Russell"
            password: "12345"
        }*/
        RemoteMessage.to_mcu.Builder mcuInfo = RemoteMessage.to_mcu.newBuilder();
        RemoteMessage.login_req.Builder loginInfo = RemoteMessage.login_req.newBuilder();
        mcuInfo.setSeq(1);
        mcuInfo.setLogin(loginInfo.setUsername("Russell"));
        mcuInfo.setLogin(loginInfo.setPassword("12345"));
        mcuInfo.build();
        Log.d(TAG,"toMcu = " + mcuInfo.toString());


        //对req_buf解码,预期数据
        toMcuBuf = new byte[]{0x08, 0x01, 0x1a, 0x10, 0x0a, 0x07, 0x52, 0x75, 0x73, 0x73, 0x65, 0x6c, 0x6c, 0x12, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35};
        RemoteMessage.to_mcu toMcu= null;
        try {
            toMcu = RemoteMessage.to_mcu.parseFrom(toMcuBuf);
            Log.d(TAG, "Username : " + String.valueOf("Russell".equals(toMcu.getLogin().getUsername())));
            Log.d(TAG, "Password : " + String.valueOf("12345".equals(toMcu.getLogin().getPassword())));
            Log.d(TAG,"toMcu = " + toMcu.toString());
            /*成功解码数据
        　　　toMcu = seq: 1
        　　　login {
            　username: "Russell"
            　password: "12345"
        　　　}*/
        } catch (InvalidProtocolBufferException e) {
            throw new RuntimeException(e);
        }
        return null;
    }

    private  RemoteMessage.from_mcu formMcu() {
        //对for_mcu数据进行编码
        /*from_mcu = seq: 2
          err_code: no_impl
          login {
          status: unknown
         }
         理论上得到如上数据结果*/
        RemoteMessage.from_mcu.Builder forMcu = RemoteMessage.from_mcu.newBuilder();
        RemoteMessage.login_res.Builder ResStatus = RemoteMessage.login_res.newBuilder();
        forMcu.setSeq(2);
        forMcu.setErrCode(RemoteMessage.remote_call_err_code.no_impl);
        forMcu.setLogin(ResStatus.setStatus(RemoteMessage.boolean_t.unknown));

        Log.d(TAG,"forMcu = " + forMcu.toString());
        //对for_mcu数据进行解码,预期数据
        byte[] expected_from_mcu_binary = {0x08, 0x02, 0x10, 0x04, 0x1a, 0x02, 0x08, 0x02};
        RemoteMessage.from_mcu fromMcuBinary= null;
        try {
            fromMcuBinary = RemoteMessage.from_mcu.parseFrom(expected_from_mcu_binary);
        } catch (InvalidProtocolBufferException e) {
            throw new RuntimeException(e);
        }
        Log.d(TAG,"from_mcu_binary = " + fromMcuBinary.toString());
        return forMcu.build();
    }
}
