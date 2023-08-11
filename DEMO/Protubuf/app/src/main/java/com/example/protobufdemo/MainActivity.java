package com.example.protobufdemo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import com.google.protobuf.InvalidProtocolBufferException;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        test();
        setContentView(R.layout.activity_main);
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
}