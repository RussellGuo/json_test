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

    IPosCard.Stub mStu = new IPosCard.Stub(){

        @Override
        public void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat, double aDouble, String aString) throws RemoteException {

        }

        @Override
        public int open(int category, Bundle bundle, IBinder cb) throws RemoteException {
            return 0;
        }

        @Override
        public void setStringData(String str) throws RemoteException {
            mStrData = str;
        }

        @Override
        public void setNfcData(byte[] _byte) throws RemoteException {
            Log.d(TAG, "setNfcData ");
            mByte = _byte;
        }

        @Override
        public byte[] getNfcData() throws RemoteException {
            Log.d(TAG, "getNfcData " );
            test();
            return mByte;
        }

        @Override
        public void registerCallback(IPosCardCallback cb) throws RemoteException {
            if (cb != null) {
                mCallbacks.register(cb);
                //startCallback();
            }
        }

        @Override
        public void unregisterCallback(IPosCardCallback cb) throws RemoteException {
            if (cb != null) {
                mCallbacks.unregister(cb);
            }
        }


    };

    void test()  {
        try {
            Test.nfc_data info = Test.nfc_data.parseFrom(mByte);
            Log.d(TAG, "test: info = " +info);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

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