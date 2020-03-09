package com.example.wearearthquakeguardian;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class WearResponderActivity extends AppCompatActivity {

    private TextView mTextView;
    private Button device1_btn, device2_btn, device3_btn;
    private ProgressBar progress_bar;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wear_responder);

        mTextView = (TextView) findViewById(R.id.text);
        progress_bar = (ProgressBar) findViewById(R.id.progressBar);
        progress_bar.setVisibility(View.VISIBLE);

        SharedPreferences sharedPref = getSharedPreferences("Earthquake_Gaurdian_User", Context.MODE_PRIVATE);
        final SharedPreferences.Editor editor = sharedPref.edit();

        device1_btn = findViewById(R.id.device1_btn);
        device2_btn = findViewById(R.id.device2_btn);
        device3_btn = findViewById(R.id.device3_btn);

        device1_btn.setText("[FR]Samsung S9+");
        device2_btn.setText("[C]Samsung A50");
        device3_btn.setText("[C]Samsung S10+");

        device1_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(WearResponderActivity.this, WearCompassActivity.class);
                editor.putString("Device", "[FR]Samsung S9+");
                editor.commit();
                intent.putExtra("Lat", 43.60789814);
                intent.putExtra("Long", -79.63058887);
                startActivity(intent);
                //finish();
            }
        });

        device2_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(WearResponderActivity.this, WearCompassActivity.class);
                editor.putString("Device", "[C]Samsung A50");
                editor.commit();
                intent.putExtra("Lat", 43.46834742);
                intent.putExtra("Long", -79.69926077);
                startActivity(intent);
                //finish();
            }
        });

        device3_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(WearResponderActivity.this, WearCompassActivity.class);
                editor.putString("Device", "[C]Samsung S10+");
                editor.commit();
                intent.putExtra("Lat", 43.47842574);
                intent.putExtra("Long", -79.72724818);
                startActivity(intent);
                //finish();
            }
        });


    }
}
