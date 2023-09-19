// IPosCallback.aidl
package com.huaqin.posservices;

// Declare any non-default types here with import statements

interface IPosCallback {
    void onLoginRes(out byte[] _byte);

    void onLogoutRes(out byte[] _byte);

    void onVersionInfoRes(out byte[] _byte);

    void onKeyRes(out byte[] _byte);

    void onLogRes(out byte[] _byte);

    void onErrorRes(out byte[] _byte);

}