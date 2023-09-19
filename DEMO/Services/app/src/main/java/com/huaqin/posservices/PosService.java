
package com.huaqin.posservices;



import android.content.Context;
import android.os.RemoteCallbackList;
import android.os.RemoteException;
import android.util.Log;

import com.huaqin.posservices.remotemessage.RemoteMessageApi;

import java.nio.charset.StandardCharsets;


public class PosService extends IPosServices.Stub {
    private String TAG = "AIDL-PosService";
    private Context mContext;
    private RemoteCallbackList<IPosCallback> mCallbacks = new RemoteCallbackList<IPosCallback>();

    public PosService() {
    }

    public RemoteCallbackList<IPosCallback> getmCallbacks() {
        return mCallbacks;
    }

    @Override
    public boolean setLogin(byte[] _byte) throws RemoteException {
        return false;
    }

    @Override
    public boolean setLogout(byte[] _byte) throws RemoteException {
        return false;
    }

    @Override
    public boolean getVersionInfo(byte[] _byte) throws RemoteException {
        return false;
    }

    @Override
    public void registerCallback(IPosCallback cb) throws RemoteException {
        if (cb != null) {
            mCallbacks.register(cb);
        }
    }

    @Override
    public void unregisterCallback(IPosCallback cb) throws RemoteException {
        if (cb != null) {
            mCallbacks.unregister(cb);
        }
    }
}
