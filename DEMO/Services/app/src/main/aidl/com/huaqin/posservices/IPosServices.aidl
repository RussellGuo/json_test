// IPosServices.aidl
package com.huaqin.posservices;

// Declare any non-default types here with import statements
import com.huaqin.posservices.IPosCallback;
interface IPosServices {
    boolean setLogin(in byte[] _byte);

    boolean setLogout(in byte[] _byte);

    boolean getVersionInfo(in byte[] _byte);

    void registerCallback(IPosCallback cb);

    void unregisterCallback(IPosCallback cb);
}