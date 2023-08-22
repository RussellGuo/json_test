// IReadCardCallback.aidl
package com.huaqin.posservices;

// Declare any non-default types here with import statements

interface IReadCardCallback {
    void onError(int code, String message);
    void onCardDetected(String message);
}