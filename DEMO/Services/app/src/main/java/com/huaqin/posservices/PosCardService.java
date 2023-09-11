package com.huaqin.posservices;

import static com.example.protobufdemo.RemoteMessage.boolean_t.failed;
import static com.example.protobufdemo.RemoteMessage.boolean_t.succeeded;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;

import com.example.protobufdemo.RemoteMessage;
import com.example.protobufdemo.Test;
import com.google.protobuf.InvalidProtocolBufferException;
import com.huaqin.posservices.remotemessage.RemoteMessageApi;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;


public class PosCardService extends Service {
    private String TAG = "AIDL-PosCardService";
    private String mStrData;
    private Boolean mSetServicesRuning = false;
    private byte[] mByte;
    private byte[] mLoginByte;
    private byte[] mLoginResult;

    private HashMap<IBinder, IReadCardCallback> mListenersMap;
    public PosCardService() {
        mListenersMap = new HashMap<IBinder, IReadCardCallback>();
    }

    final RemoteCallbackList<IReadCardCallback> mCallbacks = new RemoteCallbackList<IReadCardCallback>();

    /**
     *
     */
    IPosCard.Stub mStu = new IPosCard.Stub(){

        @Override
        public void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat, double aDouble, String aString) throws RemoteException {

        }

        /**
         *　测试方法，暂未使用
         * @param category
         * @param bundle
         * @param cb
         * @return
         * @throws RemoteException
         */
        @Override
        public int open(int category, Bundle bundle, IBinder cb) throws RemoteException {
            return 0;
        }

        /**
         *测试普通字符串
         * @param str
         * @throws RemoteException
         */
        @Override
        public void setStringData(String str) throws RemoteException {
            mStrData = str;
        }

        /**
         *设置测试数据
         * @param _byte
         * @throws RemoteException
         */
        @Override
        public void setNfcData(byte[] _byte) throws RemoteException {
            Log.d(TAG, "setNfcData ");
            mByte = _byte;
        }

        /**
         *获取测试数据
         * @return
         * @throws RemoteException
         */
        @Override
        public byte[] getNfcData() throws RemoteException {
            Log.d(TAG, "getNfcData " );
            test();
            return mByte;
        }


        /**
         * 模拟登录方法
         * @param _byte
         * @throws RemoteException
         */
        @Override
        public void setLogin(byte[] _byte) throws RemoteException {
            getLoginTest(_byte);
            //mLoginByte = _byte;
        }

        /**
         *获取登录数据
         * @return
         * @throws RemoteException
         */
        @Override
        public byte[] getLogin() throws RemoteException {
            Log.d(TAG, "getLogin " );
            return mLoginResult;
        }

        @Override
        public void checkCard() throws RemoteException {
            Log.d(TAG, "Start checkCard" );
            try {
                Thread.sleep(2000);
            } catch (InterruptedException e) {
                throw new RuntimeException(e);
            }
            byte[]  to_mcu_buf = {0x08, 0x01, 0x1a, 0x10, 0x0a, 0x07, 0x52, 0x75, 0x73, 0x73, 0x65, 0x6c, 0x6c, 0x12, 0x05, 0x31, 0x32, 0x33, 0x34, 0x35};
            new RemoteMessageApi().fotMCUServices(to_mcu_buf,mCallbacks);
           // startCallback();
        }

        @Override
        public void registerCallback(IReadCardCallback cb) throws RemoteException {
            if (cb != null) {
                synchronized (mListenersMap) {
                    mListenersMap.put(cb.asBinder(), cb);
                }
                mCallbacks.register(cb);
                //startCallback();
            }
        }

        @Override
        public void unregisterCallback(IReadCardCallback cb) throws RemoteException {
            synchronized (mListenersMap) {
                mListenersMap.remove(cb.asBinder());
            }
            if (cb != null) {
                mCallbacks.unregister(cb);
            }
        }


    };

    /**
     * 测试protobuf数据反序列化
     */
    void test()  {
        try {
            //通过对应的proto文件创建的类进行反序列化操作，
            Test.nfc_data info = Test.nfc_data.parseFrom(mByte);
            //把反序列化后对象转化为proto　Builder对象可操作数据内容
            Test.nfc_data.Builder build = info.toBuilder();
            //更改从客户端传输数据中的某一个字段值，返回给客户端
            build.setOPEN("services open");
            mByte = build.build().toByteArray();
            Log.d(TAG, "test: info = " +build.build());
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    void getLoginTest(byte[] _byte){
        try {
            //解析收到的账号与密码，反序列化
            RemoteMessage.to_mcu info =  RemoteMessage.to_mcu.parseFrom(_byte);
            String userName = info.getLogin().getUsername();
            String password = info.getLogin().getPassword();
            //创建返回结果对象
            RemoteMessage.from_mcu.Builder result = RemoteMessage.from_mcu.newBuilder();
            //判断账号密码
            if("admin".equals(userName) && "admin".equals(password)){
                //设置返回结果－登录成功
                result.setSeq(1)
                        .setLogin(RemoteMessage.login_res.newBuilder()
                                .setStatus(succeeded));
                Log.d(TAG,"login pass = " + result.toString());
            }else{
                //设置返回结果－登录失败
                result.setSeq(2)
                        .setLogin(RemoteMessage.login_res.newBuilder()
                                .setStatus(failed));
                Log.d(TAG,"login fail = " + result.toString());
            }
            //将结果对象序列化
            mLoginResult = result.build().toByteArray();
        } catch (InvalidProtocolBufferException e) {
            throw new RuntimeException(e);
        }

    }

    /**
     * 启动回调方法
     */
   public void startCallback(RemoteCallbackList<IReadCardCallback> mCallbacks) {
        final int N = mCallbacks.beginBroadcast();
        Log.d(TAG, "------------------ callback hello world 123  N = " + N);
        for (int i = 0; i < N; i++) {
            try {
                Log.d(TAG, "callback hello world 123");
                mCallbacks.getBroadcastItem(i).onCardDetected(" Read A Card Success");
            } catch (RemoteException e){
                e.printStackTrace();
            }

        }
        mCallbacks.finishBroadcast();
    }

    @Override
    public void onCreate() {
        super.onCreate();
    }

    @Override
    public void onStart(Intent intent, int startId) {
        Log.v(TAG, "onStart()");
    }


    /**
     *
     * @param intent The Intent that was used to bind to this service,
     * as given to {@link android.content.Context#bindService
     * Context.bindService}.  Note that any extras that were included with
     * the Intent at that point will <em>not</em> be seen here.
     *
     * @return
     */
    @Override
    public IBinder onBind(Intent intent) {
        Log.v(TAG, "onBind()");
        mSetServicesRuning = true;
        new Thread(){
            @Override
            public void run() {
                super.run();
                while (mSetServicesRuning) {
                    try {
                        Thread.sleep(1000);
                        Log.v(TAG,"mStrData = " +mStrData);
                        //startCallback();
                    } catch (InterruptedException e) {
                        throw new RuntimeException(e);
                    }
                }
            }
        }.start();
        return mStu;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        Log.v(TAG, "onUnbind()");
        mSetServicesRuning = false;
        return true;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.v(TAG, "onDestroy()");
    }



}