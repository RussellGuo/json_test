package com.huaqin.posservices;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    private String TAG = "JNI-TEST";
    private Button mInitUartButton;
    private Button mDatagramSendButton;
    private Toast toast = null;
    private Context mContext;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mContext = this;
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        mInitUartButton = findViewById(R.id.button);
        mDatagramSendButton = findViewById(R.id.button2);
        mInitUartButton.setOnClickListener(this);
        mDatagramSendButton.setOnClickListener(this);
        //tv.setText(? "true" : "false");
    }


    /**
     * A native method that is implemented by the 'serialport' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    Thread thread = new Thread() {
        @Override
        public void run() {
            SerialPort.serialDatagramReceiveLoop();
        }
    };


    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.button:
                Log.d(TAG, "init uart");
                boolean bel = SerialPort.initUart();
                Log.d(TAG, "init uart = " + bel);
                Toast.makeText(this, "init uart = " + bel, Toast.LENGTH_SHORT).show();
                break;
            case R.id.button2:
                Log.d(TAG, "serial Datagram Send ");
                //thread.start();
                new Thread() {
                    @Override
                    public void run() {
                        SerialPort.serialDatagramReceiveLoop();
                    }
                }.start();
                boolean rel;
                byte[]  to_mcu_buf = {0x07, 0x63, 0x61, 0x6F, 0x6D, 0x65, 0x6E, 0x67, 0x12, 0x06, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36};
                rel =  SerialPort.serialDatagramSend(to_mcu_buf);
                Log.d(TAG,"rel = " + rel);

                break;
        }
    }

    public void onResultCallBack(int obj) {
        Log.e( TAG, "onResultCallBack: " + obj);
        // thread.stop();
    }
}