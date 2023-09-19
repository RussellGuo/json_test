// IPosCallback.aidl
package com.huaqin.posservices;

// Declare any non-default types here with import statements

interface IPosCallback {
    void loginRes(out byte[] _byte);

    void logoutRes(out byte[] _byte);

    void versionInfoRes(out byte[] _byte);

}