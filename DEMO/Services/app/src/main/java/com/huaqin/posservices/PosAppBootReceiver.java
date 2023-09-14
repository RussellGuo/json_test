package com.huaqin.posservices;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class PosAppBootReceiver extends BroadcastReceiver {
    private static final String TAG = "PosAppBootReceiver";

    @Override
    public void onReceive(Context context, Intent intent) {
        String action = intent.getAction();

        Log.d(TAG, "abc onReceive action=" + action);

        if (action.equals(Intent.ACTION_BOOT_COMPLETED)) {
            context.startService(new Intent(context, PosCardService.class));
        }
    }
}
