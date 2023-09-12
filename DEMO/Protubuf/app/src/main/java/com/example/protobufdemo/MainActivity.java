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
import android.widget.Toast;

import com.example.protobufdemo.remotemessage.RemoteMessageApi;
import com.google.protobuf.InvalidProtocolBufferException;
import com.huaqin.posservices.IPosCard;
import com.huaqin.posservices.IReadCardCallback;

public class MainActivity extends AppCompatActivity implements View.OnClickListener,ServiceConnection {

    private static final String TAG = "MainActivity";
    private Intent mPosServices;

    private IPosCard mBinder;
    private int mICount = 0;
    byte[] nfc;
    private Boolean binService = false;

    private Context mContext = null;
    private Toast toast = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mContext = this;
        //创建Intent 设置需要连接的服务
        mPosServices = new Intent();
        test1();
        mPosServices.setComponent(new ComponentName("com.huaqin.posservices","com.huaqin.posservices.PosCardService"));
        setContentView(R.layout.activity_main);
        findViewById(R.id.button).setOnClickListener(this);
        findViewById(R.id.button2).setOnClickListener(this);
        findViewById(R.id.button3).setOnClickListener(this);
        findViewById(R.id.button4).setOnClickListener(this);
        findViewById(R.id.button5).setOnClickListener(this);
        findViewById(R.id.button6).setOnClickListener(this);
        findViewById(R.id.button7).setOnClickListener(this);

        RemoteMessageApi api = new RemoteMessageApi();
        RemoteMessage.login_req.Builder loginInfo = RemoteMessage.login_req.newBuilder();
        loginInfo.setUsername("caomeng");
        loginInfo.setPassword("123456");
        Log.d(TAG,"loginInfo = " + loginInfo.build().toByteArray());
        byte[] atr = loginInfo.build().toByteArray();
        for (byte str:
        atr) {
            Log.d(TAG,"loginInfo = " + str);
        }
        byte[]  to_mcu_buf = {0x07, 0x63, 0x61, 0x6F, 0x6D, 0x65, 0x6E, 0x67, 0x12, 0x06, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36};
        for (byte str:
                to_mcu_buf) {
            Log.d(TAG,"loginInfo22 = " + str);
        }
        RemoteMessage.logout_req.Builder logOUTInfo = RemoteMessage.logout_req.newBuilder();
        logOUTInfo.setDummy(1);
        api.remoteCllService(loginInfo.build());
        api.remoteCllService(logOUTInfo.build());
    }


    /**
     * 初始化proto数据，方便测试
     * @return　输出proto反序列化后数据
     */
    public byte[] test(){
        //创建proto对象
        Test.nfc_data.Builder build =  Test.nfc_data.newBuilder();
        //初始化数据
        build.setId(1);
        build.setOPEN("open nfc");
        build.setRand("read nfc type");
        build.setColse("colse nfc");
        Test.nfc_data info = build.build();
        return info.toByteArray();
    }

    /**
     * 创建登录数据
     * @return
     */
    public byte[] testLogin(){
        Log.d(TAG,"buildLogin");
        //发给mcu　proto数据对象
        RemoteMessage.to_mcu.Builder build = RemoteMessage.to_mcu.newBuilder();
        //账号密码proto数据对象
        RemoteMessage.login_req.Builder buildLogin = RemoteMessage.login_req.newBuilder();
        build.setLogin(buildLogin.setUsername("caomeng")
                        .setPassword("123456"));
                //.setCrc(2);
        Log.d(TAG,"buildLogin = " + build.build());
        byte[] ss=build.build().toByteArray();
        for (byte s:
        ss) {
            Log.d(TAG,"buildLogin = " + s);
        }
        try {
            RemoteMessage.to_mcu toMcu = RemoteMessage.to_mcu.parseFrom(ss);
            Log.d(TAG,"buildLogin = " + toMcu.toString());
        } catch (InvalidProtocolBufferException e) {
            throw new RuntimeException(e);
        }
        return build.build().toByteArray();
    }


    private void test1(){
        byte[]  to_mcu_buf = {0x1A, 0x11, 0x0A, 0x07, 0x63, 0x61, 0x6F, 0x6D, 0x65, 0x6E, 0x67, 0x12, 0x06, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36};
        byte[] expected_from_mcu_binary = {0x08, 0x02, 0x10, 0x04, 0x1a, 0x02, 0x08, 0x02};
        try {
            RemoteMessage.to_mcu toMcu=   RemoteMessage.to_mcu.parseFrom(to_mcu_buf);
            Log.d(TAG,"toMcu = " + toMcu.toString());

            RemoteMessage.from_mcu fromMcu=   RemoteMessage.from_mcu.parseFrom(expected_from_mcu_binary);
            Log.d(TAG,"from_mcu = " + fromMcu.toString());
        } catch (InvalidProtocolBufferException e) {
            throw new RuntimeException(e);
        }


    }


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.button:
                Log.d(TAG,"bind PosServices");
                //绑定服务
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
                    //把proto反序列化后数据传给services
                    mBinder.setNfcData(test());
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
                break;
            case R.id.button4:
                Log.d(TAG,"GET NFC DATA");
                try {
                    //调用services 方法获取服务端byte数据
                    byte[] nfcdata = mBinder.getNfcData();
                    //对获取到的数据进行序列化
                    Test.nfc_data info = Test.nfc_data.parseFrom(nfcdata);
                    toast = Toast.makeText(mContext, info.toString(), Toast.LENGTH_SHORT);
                    toast.show();
                    Log.d(TAG, "test: info = " +info);
                } catch (RemoteException | InvalidProtocolBufferException e) {
                    throw new RuntimeException(e);
                }
                break;
            case R.id.button5:
                Log.d(TAG,"LOGIN");
                try {
                    //把proto反序列化后数据传给services
                    mBinder.setLogin(testLogin());
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
                break;
            case R.id.button6:
                Log.d(TAG,"GET LOGIN RESULT");
                try {
                    //调用services 方法获取服务端byte数据
                    byte[] loginResult = mBinder.getLogin();
                    //对获取到的数据进行序列化
                    RemoteMessage.from_mcu info = RemoteMessage.from_mcu.parseFrom(loginResult);
                    toast = Toast.makeText(mContext, "LOGIN " + info.getLogin(), Toast.LENGTH_SHORT);
                    toast.show();
                    Log.d(TAG, "test: info = " +info.getLogin());
                } catch (RemoteException | InvalidProtocolBufferException e) {
                    throw new RuntimeException(e);
                }
                break;
            case R.id.button7:
                Log.d(TAG,"CHECK CARD");
                try {
                    mBinder.checkCard();
                } catch (RemoteException e) {
                    throw new RuntimeException(e);
                }
                break;
        }
    }

    /**
     *
     * @param name The concrete component name of the service that has
     * been connected.
     *
     * @param service The IBinder of the Service's communication channel,
     * which you can now make calls on.
     */
    @Override
    public void onServiceConnected(ComponentName name, IBinder service) {
        if(mBinder == null) {
            mBinder = IPosCard.Stub.asInterface(service);
            mICount ++;
            Log.v(TAG,"第" + mICount + "次连接服务！");
            binService = true;
            try {
                String strData = "第" + mICount + "次连接Service成功！";
                mBinder.setStringData(strData);
                mBinder.registerCallback(readCardCallback);
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

    IReadCardCallback readCardCallback = new IReadCardCallback.Stub(){

        @Override
        public void onError(int i, String s) throws RemoteException {

        }

        @Override
        public void onCardDetected(String s) throws RemoteException {
            toast = Toast.makeText(mContext, s, Toast.LENGTH_SHORT);
            toast.show();
            Log.d(TAG,"Read card result = " + s);
        }
    };
}