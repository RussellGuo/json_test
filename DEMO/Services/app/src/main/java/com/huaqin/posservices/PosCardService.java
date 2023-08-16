package com.huaqin.posservices;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;

import androidx.annotation.Nullable;

import com.example.protobufdemo.Test;
import com.google.protobuf.InvalidProtocolBufferException;


public class PosCardService extends Service {
    private String TAG = "AIDL-PosCardService";
    private String mStrData;
    private Boolean mSetServicesRuning = false;
    private byte[] mByte;
    public PosCardService() {
    }

    final RemoteCallbackList<IPosCardCallback> mCallbacks = new RemoteCallbackList<IPosCardCallback>();

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
         *注册回调
         * @param cb
         * @throws RemoteException
         */
        @Override
        public void registerCallback(IPosCardCallback cb) throws RemoteException {
            if (cb != null) {
                mCallbacks.register(cb);
                startCallback();
            }
        }

        /**
         *取消注册
         * @param cb
         * @throws RemoteException
         */
        @Override
        public void unregisterCallback(IPosCardCallback cb) throws RemoteException {
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

    /**
     * 启动回调方法
     */
    void startCallback() {
        final int N = mCallbacks.beginBroadcast();
        Log.d(TAG, "------------------ callback hello world 123");
        for (int i = 0; i < N; i++) {
            try {
                Log.d(TAG, "callback hello world 123");
                mCallbacks.getBroadcastItem(i).serverReport("hello world");
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