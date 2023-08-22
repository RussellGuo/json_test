// IPosCard.aidl
package com.huaqin.posservices;
// Declare any non-default types here with import statements
import com.huaqin.posservices.IReadCardCallback;

interface IPosCard {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat,
            double aDouble, String aString);

    int open(int category, in Bundle bundle, in IBinder cb);

    void setStringData(String str);

    void setNfcData(in byte[] _byte);

    byte[] getNfcData();

    void setLogin(in byte[] _byte);

    byte[] getLogin();

    void checkCard();

    void registerCallback(IReadCardCallback cb);

    void unregisterCallback(IReadCardCallback cb);
}