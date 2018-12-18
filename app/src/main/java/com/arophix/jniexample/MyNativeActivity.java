package com.arophix.jniexample;

import android.app.NativeActivity;
import android.os.Bundle;

import dalvik.system.BaseDexClassLoader;

public class MyNativeActivity extends NativeActivity {
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    
        String libname = "native-me";
        
        BaseDexClassLoader classLoader = (BaseDexClassLoader) getClassLoader();
        String path = classLoader.findLibrary(libname);
        
        if (path == null) {
            throw new IllegalArgumentException("Unable to find native library " + libname + " using classloader: " + classLoader.toString());
        }
        
    }

}
