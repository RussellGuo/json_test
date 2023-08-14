package com.huaqin.posservices;

import android.app.Service;
import android.content.Intent;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;

import com.example.protobufdemo.IPosCard;

public class PosCardService extends Service {
    private String TAG = "AIDL-PosCardService";
    private String mStrData;
    private Boolean mSetServicesRuning = false;
    private byte[] mByte;
    public PosCardService() {
    }

    IPosCard.Stub mStu = new IPosCard.Stub() {
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
            mByte = _byte;
        }
    };

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