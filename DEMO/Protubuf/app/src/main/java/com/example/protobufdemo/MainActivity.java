package com.example.protobufdemo;

import androidx.appcompat.app.AppCompatActivity;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.google.protobuf.InvalidProtocolBufferException;

public class MainActivity extends AppCompatActivity implements View.OnClickListener,ServiceConnection {

    private static final String TAG = "MainActivity";
    private Intent mPosServices;

    private Button button1,button2;

    private IPosCard mBinder;
    private int mICount = 0;
    private Boolean binService = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        test();
        mPosServices = new Intent();
        mPosServices.setComponent(new ComponentName("com.huaqin.posservices","com.huaqin.posservices.PosCardService"));
        setContentView(R.layout.activity_main);
        findViewById(R.id.button).setOnClickListener(this);
        findViewById(R.id.button2).setOnClickListener(this);
    }


    public void test(){
        Test.nfc_data.Builder build =  Test.nfc_data.newBuilder();
        build.setId(1);
        build.setOPEN("20191018105706");
        build.setRand("123456789");
        build.setColse("0987654321");
        Test.nfc_data info = build.build();
        byte[] nfc = info.toByteArray();
        Log.d(TAG, "test: nfc = " +nfc);
        try {
            info =  Test.nfc_data.parseFrom(nfc);
            Log.d(TAG, "test: nfc2 = " +nfc);
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.button:
                Log.d(TAG,"bind PosServices");
                bindService(mPosServices, this, Context.BIND_AUTO_CREATE);
                break;
            case R.id.button2:
                Log.d(TAG,"unbind PosServices");
                if (binService) {
                    unbindService( this);
                }
                break;
        }
    }


    @Override
    public void onServiceConnected(ComponentName name, IBinder service) {
        if(mBinder == null) {
            mBinder = IPosCard.Stub.asInterface(service);
            mICount ++;
            Log.v(TAG,"第" + mICount + "次连接服务！");
            try {
                binService = true;
                String strData = "第" + mICount + "次连接Service成功！";
                mBinder.setStringData(strData);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void onServiceDisconnected(ComponentName name) {
        Log.v(TAG,"onServiceDisconnected");
    }
}