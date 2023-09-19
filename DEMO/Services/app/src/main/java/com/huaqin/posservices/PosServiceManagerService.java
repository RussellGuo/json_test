package com.huaqin.posservices;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;

import com.huaqin.posservices.remotemessage.RemoteMessageApi;

/**
 * @author ubuntu
 */
public class PosServiceManagerService extends Service {
    private String TAG = "AIDL-PosServiceManagerService";
    private static PosService mService;
    private static PosServiceManagerService mInstance;

    public PosServiceManagerService() {
    }
    public static synchronized PosServiceManagerService init() {
        if (mInstance == null) {
            mInstance = new PosServiceManagerService();
        }
        if(mService == null){
            mService = new PosService();
        }
        return mInstance;
    }

    public PosService getPosService() {
        return mService;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.v(TAG, "onCreate()");
    }

    @Override
    public void onStart(Intent intent, int startId) {
        Log.v(TAG, "onStart()");
    }

    @Override
    public IBinder onBind(Intent intent) {
        // TODO: Return the communication channel to the service.
        Log.v(TAG, "onBind() = " + mService);
        return mService;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        Log.v(TAG, "onUnbind()");
        return true;
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.v(TAG, "onDestroy()");
    }
}