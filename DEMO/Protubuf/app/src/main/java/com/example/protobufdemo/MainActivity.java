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

import com.google.protobuf.InvalidProtocolBufferException;
import com.huaqin.posservices.IPosCard;

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
        mPosServices.setComponent(new ComponentName("com.huaqin.posservices","com.huaqin.posservices.PosCardService"));
        setContentView(R.layout.activity_main);
        findViewById(R.id.button).setOnClickListener(this);
        findViewById(R.id.button2).setOnClickListener(this);
        findViewById(R.id.button3).setOnClickListener(this);
        findViewById(R.id.button4).setOnClickListener(this);
        findViewById(R.id.button5).setOnClickListener(this);
        findViewById(R.id.button6).setOnClickListener(this);
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
        build.setSeq(1)
                .setLogin(buildLogin.setUsername("admin")
                        .setPassword("admin"))
                .setCrc(2);
        Log.d(TAG,"buildLogin = " + build.build());
        return build.build().toByteArray();
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