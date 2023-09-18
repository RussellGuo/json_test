// IPosCallback.aidl
package com.huaqin.posservices;

// Declare any non-default types here with import statements

interface IPosCallback {
    void onError(int code, String message);
    void onCardDetected(String message);
}