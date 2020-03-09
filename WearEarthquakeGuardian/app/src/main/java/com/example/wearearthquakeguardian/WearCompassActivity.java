package com.example.wearearthquakeguardian;

import android.Manifest;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.hardware.GeomagneticField;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.provider.Settings;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.RotateAnimation;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class WearCompassActivity extends AppCompatActivity implements SensorEventListener {

    private TextView mTextView;
    private ImageView arrow;
    private ImageView compass;
    private Button alarm_btn, send_btn;
    private int alarm_int = 0;
    private float curDegree = 0f;
    private float curDegreeForN = 0f;

    private Context context;

    private double myLat;
    private double myLon;

    private LocationManager locationManager;
    private static SensorManager sensorManager;
    private Sensor sensor;

    private Double doubleTargetLat;
    private Double doubleTargetLon;

    private Location myLocation = new Location("me");
    private Location target = new Location("target");

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_wear_compass);

        arrow = findViewById(R.id.arrow);
        compass = findViewById(R.id.compass);
        alarm_btn = findViewById(R.id.alarm_btn);
        send_btn = findViewById(R.id.send_btn);

        mTextView = (TextView) findViewById(R.id.text);

        SharedPreferences sharedPref = getSharedPreferences("Earthquake_Gaurdian_User", Context.MODE_PRIVATE);

        Intent intent = getIntent();

        mTextView.setText(sharedPref.getString("Device", "[N/A]"));

        alarm_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(alarm_int == 0) {
                    alarm_int = 1;
                    Toast.makeText(WearCompassActivity.this, "Alarm Enabled!", Toast.LENGTH_SHORT).show();
                    //alarm_btn.setText("Enable Alarm");
                }
                else{
                    alarm_int = 0;
                    Toast.makeText(WearCompassActivity.this, "Alarm Disabled!", Toast.LENGTH_SHORT).show();
                    //alarm_btn.setText("Disable Alarm");
                }
            }
        });

        send_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(WearCompassActivity.this, WearMessageActivity.class);
                startActivity(intent);
            }
        });

        doubleTargetLat = Double.valueOf(intent.getDoubleExtra("Lat",0.0));
        doubleTargetLon = Double.valueOf(intent.getDoubleExtra("Long",0.0));

        target.setLatitude(doubleTargetLat);
        target.setLongitude(doubleTargetLon);

        // Setup the location manager to get the device location
        locationManager = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
        LocationListener locationListenerGPS = new LocationListener() {
            @Override
            public void onLocationChanged(Location location) {
                myLat = location.getLatitude();
                myLon = location.getLongitude();
                myLocation = location;
            }

            @Override
            public void onStatusChanged(String provider, int status, Bundle extras) {

            }

            @Override
            public void onProviderEnabled(String provider) {

            }

            @Override
            public void onProviderDisabled(String provider) {

            }
        };
        if (checkSelfPermission(Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED && checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    Activity#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for Activity#requestPermissions for more details.
            return;
        }
        locationManager.requestLocationUpdates(LocationManager.GPS_PROVIDER, 1000, 1, locationListenerGPS);
        isLocationEnabled();

        // Setup orientation sensor
        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        sensor = sensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION);
    }

    private void isLocationEnabled() {

        if (!locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
            AlertDialog.Builder alertDialog = new AlertDialog.Builder(context);
            alertDialog.setTitle("Enable Location");
            alertDialog.setMessage("Your locations setting is not enabled. Please enabled it in settings menu.");
            alertDialog.setPositiveButton("Location Settings", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
                    startActivity(intent);
                }
            });
            alertDialog.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int which) {
                    dialog.cancel();
                }
            });
            AlertDialog alert = alertDialog.create();
            alert.show();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        //Keep the sensor listening
        if (sensor != null) {
            sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_GAME);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();

        //Stop the sensor
        sensorManager.unregisterListener(this);
    }


    @Override
    public void onSensorChanged(SensorEvent event) {

        // Get the angle between my device and north
        float degree = Math.round(event.values[0]);
        float degreeForN = degree;

        // Calculate the angle between my device to the target device
        if (myLocation != null) {
            float bearing = myLocation.bearingTo(target);
            GeomagneticField geomagneticField =
                    new GeomagneticField(Double.valueOf(myLat).floatValue(),
                            Double.valueOf(myLon).floatValue(),
                            Double.valueOf(myLocation.getAltitude()).floatValue(),
                            System.currentTimeMillis());
            degree -= geomagneticField.getDeclination();
            if (bearing < 0) {
                bearing += 360;
            }
            degree = bearing - degree;
            if (degree < 0) {
                degree += 360;
            }
        }


            // Perform the arrow animation
            RotateAnimation rotateAnimation = new RotateAnimation(curDegree, degree,
                    Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
            rotateAnimation.setDuration(1000);
            rotateAnimation.setFillAfter(true);
            arrow.startAnimation(rotateAnimation);

            RotateAnimation rotateAnimationCompass = new RotateAnimation(curDegreeForN, (float) -degreeForN,
                    Animation.RELATIVE_TO_SELF, 0.5f, Animation.RELATIVE_TO_SELF, 0.5f);
            rotateAnimationCompass.setDuration(1000);
            rotateAnimationCompass.setFillAfter(true);
            compass.startAnimation(rotateAnimationCompass);

            curDegree = degree;
            curDegreeForN = -degreeForN;
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
    }

    @Override
    public void onPointerCaptureChanged(boolean hasCapture) {
    }
}
