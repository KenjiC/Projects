package com.example.wearearthquakeguardian;

import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class WearMessageActivity extends AppCompatActivity {

    private TextView mTextView;
    private Button message1_btn, message2_btn, message3_btn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wear_message);

        mTextView = (TextView) findViewById(R.id.text);
        message1_btn = findViewById(R.id.message1_btn);
        message2_btn = findViewById(R.id.message2_btn);
        message3_btn = findViewById(R.id.message3_btn);

        Intent intent = getIntent();

        SharedPreferences sharedPref = getSharedPreferences("Earthquake_Gaurdian_User", Context.MODE_PRIVATE);

        mTextView.setText("Inform: "+sharedPref.getString("Device", "[N/A]"));
        message1_btn.setText("We are coming!");
        message2_btn.setText("Please make some noise!");
        message3_btn.setText("PLease keep calm!");

        message1_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(WearMessageActivity.this, "Message Sent!", Toast.LENGTH_SHORT).show();
                finish();
            }
        });

        message2_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(WearMessageActivity.this, "Message Sent!", Toast.LENGTH_SHORT).show();
                finish();
            }
        });

        message3_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Toast.makeText(WearMessageActivity.this, "Message Sent!", Toast.LENGTH_SHORT).show();
                finish();
            }
        });
    }
}
