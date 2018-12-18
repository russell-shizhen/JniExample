package com.arophix.jniexample;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.provider.Settings;
import android.support.annotation.NonNull;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.telecom.TelecomManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;

import com.arophix.jniexample.jniobjects.AroMemory;
import com.arophix.jniexample.jniobjects.AroStorage;

import java.io.File;
import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel;

import static android.Manifest.permission.READ_PHONE_STATE;

public class MainActivity extends AppCompatActivity {
    
    private static final String TAG = "ArophixJava";
    
    // Used to load the 'native-lib' library on application startup. Note that the static initializer
    // will be firstly invoked before the class itself is loaded successfully.
//    static {
//        System.loadLibrary("native-lib");
//    }
    
    
    FileDescriptor fileDescriptor;
    
    
    private static final int MY_PERMISSIONS_REQUEST_READ_PHONE_STATE = 0;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    
        String path_sd_card = Environment.getExternalStorageDirectory().getAbsolutePath();
    
        FileOutputStream outputStream;
        FileInputStream inputStream;
    
        // 1. This path works.
        //System.load("/data/data/com.arophix.jniexample/files/libnative-lib.so");
        String filesDir = getFilesDir().getAbsolutePath();
        
        try {
            inputStream = new FileInputStream(new File(path_sd_card + "/Download/libnative-lib.so"));
            outputStream = new FileOutputStream(new File(filesDir + "/libnative-lib2.so"));//openFileOutput("libnative-lib2.so", Context.MODE_PRIVATE);
    
            FileChannel inChannel = inputStream.getChannel();
            FileChannel outChannel = outputStream.getChannel();
            inChannel.transferTo(0, inChannel.size(), outChannel);
            inputStream.close();
            outputStream.close();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
    
        // 2. This path works?
        System.load(filesDir + "/libnative-lib2.so");
        
//        System.load(path_sd_card + "/Download/libnative-lib.so");
        
    
//        try {
//            ApplicationInfo applicationInfo = getPackageManager().getApplicationInfo("arophix.com.androidopenssl", 0);
//            String applicationName = (String) (applicationInfo != null ? getPackageManager().getApplicationLabel(applicationInfo) : "(unknown)");
//            Log.e("SHIZHEN", "applicationName: " + applicationName);
//        } catch (PackageManager.NameNotFoundException e) {
//            e.printStackTrace();
//        }
        
        /*
         * An example of a call to a native method returning a Java string.
         */
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
    
        AroMemory aroMemory = new AroMemory();
        AroStorage aroStorage = new AroStorage("SDCard", 1);
    
        /*
         * An example of simply returning an integer from native layer.
         */
        int jniVersion = aroStorage.getJniVersion();
        Log.i(TAG, "aroStorage Jni version: " + jniVersion);
        
        /*
         * An example of a fairly complicate call to a native method which accepts a user defined
         * Java object as parameter.
         */
        String signatureString1 = aroStorage.computeStorageSignature(aroMemory);
        Log.i(TAG, "Signature 1: " + signatureString1);
        
        /*
         * An example of a fairly complicate call to a native method which accepts a user defined
         * Java object as parameter but throws a Java exception.
         */
        try {
            String signatureString2 = aroStorage.computeStorageSignatureWrongDeviceFingerprintException(aroMemory);
            Log.i(TAG, "Signature 2: " + signatureString2);
        } catch (Exception e) {
            Log.i(TAG, "Exception caught: " + e.getMessage());
        }
        
        /*
         * An example of a call to a native method which start a pthread_t and tries to access JNI functions.
         */
        aroStorage.downlaodImageNativeAsyncTask();
    
        TelephonyManager telephonyManager = (TelephonyManager) this.getSystemService(TELEPHONY_SERVICE);
        if (telephonyManager != null) {
            Log.i("TEST_UNIQUE_ID", "Build.VERSION.SDK_INT: " + Build.VERSION.SDK_INT);
    
            if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
        
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_PHONE_STATE}, MY_PERMISSIONS_REQUEST_READ_PHONE_STATE);
                    return;
                }
                String deviceId = telephonyManager.getDeviceId();
                Log.i("TEST_UNIQUE_ID", "deviceId: " + deviceId);
            } else {
                if (ActivityCompat.checkSelfPermission(this, Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED) {
                    ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_PHONE_STATE}, MY_PERMISSIONS_REQUEST_READ_PHONE_STATE);
                    return;
                }
                String deviceId = telephonyManager.getDeviceId();
                Log.i("TEST_UNIQUE_ID", "deviceId: " + deviceId);
                String meid = telephonyManager.getMeid();
                Log.i("TEST_UNIQUE_ID", "meid: " + meid);
                String imei = telephonyManager.getImei();
                Log.i("TEST_UNIQUE_ID", "imei: " + imei);
            }
        }
    
        android.provider.Settings settings;
    
        
    }
    
    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
    
        TelephonyManager telephonyManager = (TelephonyManager) this.getSystemService(TELEPHONY_SERVICE);
        
        switch (requestCode) {
            case MY_PERMISSIONS_REQUEST_READ_PHONE_STATE: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
    
                    if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {

                        String deviceId = telephonyManager.getDeviceId();
                        Log.i("TEST_UNIQUE_ID", "deviceId: " + deviceId);
                    } else {
                        String deviceId = telephonyManager.getDeviceId();
                        Log.i("TEST_UNIQUE_ID", "deviceId: " + deviceId);
                        String meid = telephonyManager.getMeid();
                        Log.i("TEST_UNIQUE_ID", "meid: " + meid);
                        String imei = telephonyManager.getImei();
                        Log.i("TEST_UNIQUE_ID", "imei: " + imei);
                    }
                } else {
                    // permission denied
                }
            }
            
        }
    }
    
    
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     * This is an example of hard linked JNI functions, i.e. Java_package_name_ClassName(JNIEnv *, jobject thiz)
     */
    public native String stringFromJNI();
}
