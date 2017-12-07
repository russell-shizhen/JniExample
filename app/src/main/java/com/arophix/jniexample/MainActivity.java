package com.arophix.jniexample;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

import com.arophix.jniexample.jniobjects.AroMemory;
import com.arophix.jniexample.jniobjects.AroStorage;

public class MainActivity extends AppCompatActivity {
    
    private static final String TAG = "ArophixJava";
    
    // Used to load the 'native-lib' library on application startup. Note that the static initializer
    // will be firstly invoked before the class itself is loaded successfully.
    static {
        System.loadLibrary("native-lib");
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
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
    }
    
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     * This is an example of hard linked JNI functions, i.e. Java_package_name_ClassName(JNIEnv *, jobject thiz)
     */
    public native String stringFromJNI();
}
