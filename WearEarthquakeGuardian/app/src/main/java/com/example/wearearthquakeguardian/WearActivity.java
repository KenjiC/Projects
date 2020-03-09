package com.example.wearearthquakeguardian;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class WearActivity extends AppCompatActivity {

    private TextView mTextView;
    private Button first_Responder_btn;

    private static final int PERMISSION_CODE = 1; //Can be any random number
    private String[] permissions = {Manifest.permission.ACCESS_FINE_LOCATION, Manifest.permission.ACCESS_COARSE_LOCATION};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wear_main);

        ActivityCompat.requestPermissions(this, permissions, PERMISSION_CODE);

        mTextView = (TextView) findViewById(R.id.text);

        first_Responder_btn = findViewById(R.id.first_btn);

        first_Responder_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(WearActivity.this, WearResponderActivity.class);
                startActivity(intent);
                //finish();
            }
        });
    }
}
