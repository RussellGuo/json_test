package com.huaqin.serialport;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.huaqin.serialport.databinding.ActivityMainBinding;


public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);;
        tv.setText(SerialPort.initUart()? "true" : "false");
    }


    /**
     * A native method that is implemented by the 'serialport' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}