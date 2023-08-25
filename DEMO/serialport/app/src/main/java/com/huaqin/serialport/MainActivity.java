package com.huaqin.serialport;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.huaqin.serialport.databinding.ActivityMainBinding;


public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    private String TAG = "JNI-TEST";

    private ActivityMainBinding binding;

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
                rel =  SerialPort.serialDatagramSend();
                Log.d(TAG,"rel = " + rel);

               /* try {
                    Thread.sleep(5000);
                    thread.stop();
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }*/
                break;
        }
    }

    public void onResultCallBack(int obj) {
        Log.e( TAG, "onResultCallBack: " + obj);
        thread.stop();
    }
}