package com.arophix.jniexample.jniobjects;

import android.util.Log;

/**
 * Created by shizzhan on 3/12/17.
 */
public class AroStorage implements AroObject {
    
    private static final String TAG = "ArophixJava";
    
    // Primitive int type
    private int mId;
    
    // Object type
    private String mName;
    
    // Static object type
    private static String sAroStroageDescriptor = "I am an arophix storage object defined from Java world.";
    
    /*
     * An example that shows how to invoke a Java method with primitive arguments and Object type from JNI.
     */
    private static String getAroStroageDescriptor(int arg1, float arg2, String arg3) {
        Log.i(TAG, "arg1:" + arg1 + ", arg2:" + arg2 + ", arg3:" + arg3);
        return sAroStroageDescriptor + "(via static method.)";
    }
    
    // Array type
    private byte[] storageFingerprintData = new byte[] {
            (byte)0x14, (byte)0x12, (byte)0x4c, (byte)0xd6,
            (byte)0x04, (byte)0x29, (byte)0x0a, (byte)0x4b,
            (byte)0x18, (byte)0xb2, (byte)0xd4, (byte)0xd3,
            (byte)0xb4, (byte)0x08, (byte)0x99, (byte)0x08
    };
    
    // Array type
    private byte[] storageFingerprintDataWrong = new byte[] {
            (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
            (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00,
            (byte)0x00, (byte)0x00, (byte)0x00, (byte)0x00
    };
    
    // Constructor to be accessed from native
    public AroStorage(String mName, int mId) {
        this.mName = mName;
        this.mId = mId;
    }
    
    @Override
    public String getFingerprint() {
        return this.getClass().getSimpleName();
    }
    
    public String getName() {
        return mName;
    }
    
    public void setName(String mName) {
        this.mName = mName;
    }
    
    public int getId() {
        return mId;
    }
    
    public String getImageUrl() {
        return "https://www.fake.com/image/id=123";
    }
    
    public void setId(int mId) {
        this.mId = mId;
    }
    
    public String computeStorageSignature(AroMemory aroMemory) {
        String otpString = null;
        try {
            otpString = computeStorageSignatureNative(aroMemory, storageFingerprintData);
        } catch (Exception e) {
            //e.printStackTrace();
        }
        
        for (int i = 0; i < storageFingerprintData.length; i++) {
            Log.i(TAG, "" + String.format("array[%d]: 0x%02x", i, storageFingerprintData[i]));
        }
        
        return otpString;
    }
    
    public String computeStorageSignatureWrongDeviceFingerprintException(AroMemory aroMemory) throws Exception {
        return computeStorageSignatureNative(aroMemory, storageFingerprintDataWrong);
    }
    
    // Exception from native method
    private native String computeStorageSignatureNative(AroMemory aroMemory, byte[] storageFingerprintData) throws Exception;
    
    // Get JNI version
    public native int getJniVersion();
    
    // Download image using native pthread_t
    public native int downlaodImageNativeAsyncTask();
    
}
