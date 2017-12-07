package com.arophix.jniexample.jniobjects;

/**
 * Created by shizzhan on 3/12/17.
 */

public class AroMemory implements AroObject {
    
    public int getId() {
        return 999;
    }
    
    @Override
    public String getFingerprint() {
        return "1234567890";
    }
    
    public native String getNameFromNative();
    
}
