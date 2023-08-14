// IPosCard.aidl
package com.example.protobufdemo;

// Declare any non-default types here with import statements

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
}