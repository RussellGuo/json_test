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
import com.huaqin.posservices.IPosCard;

public class MainActivity extends AppCompatActivity implements View.OnClickListener,ServiceConnection {

    private static final String TAG = "MainActivity";
    private Intent mPosServices;

   // private Button button1,button2,button3,button4;

    private IPosCard mBinder;
    private int mICount = 0;
    byte[] nfc;
    private Boolean binService = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mPosServices = new Intent();
        mPosServices.setComponent(new ComponentName("com.huaqin.posservices","com.huaqin.posservices.PosCardService"));
        setContentView(R.layout.activity_main);
        findViewById(R.id.button).setOnClickListener(this);
        findViewById(R.id.button2).setOnClickListener(this);
        findViewById(R.id.button3).setOnClickListener(this);
        findViewById(R.id.button4).setOnClickListener(this);
    }


    public byte[] test(){
        Test.nfc_data.Builder build =  Test.nfc_data.newBuilder();
        build.setId(1);
        build.setOPEN("open nfc");
        build.setRand("read nfc type");
        build.setColse("colse nfc");
        Test.nfc_data info = build.build();
        return info.toByteArray();
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
            case R.id.button3:
                Log.d(TAG,"SET NFC DATA");
                try {
                    mBinder.setNfcData(test());
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
                break;
            case R.id.button4:
                Log.d(TAG,"GET NFC DATA");
                try {
                    byte[] nfcdata = mBinder.getNfcData();
                    Test.nfc_data info = Test.nfc_data.parseFrom(nfcdata);
                    Log.d(TAG, "test: info = " +info);
                } catch (RemoteException | InvalidProtocolBufferException e) {
                    throw new RuntimeException(e);
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
            binService = true;
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
        mBinder = null;
        binService = false;
        Log.v(TAG,"onServiceDisconnected");
    }
}