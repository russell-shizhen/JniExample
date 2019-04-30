#include <jni.h>
#include "arophix_jni.h"
#include <string>
#include "stdlib.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

#ifdef __cplusplus
extern "C" {
#endif

UnionJNIEnvToVoid g_uenv;
JavaVM* g_jvm;
jclass g_aroStorageClazz;
jobject g_aroStorageObj;

static void downlaodImageNativeAsyncTask(JNIEnv *env,
                                         jobject aroStorage /*thiz is referring to AroStorage instance*/);

void* download_image_func(void *arg);


//std::string convertJStringToString(JNIEnv *env, jstring str) {
//    const jsize len = env->GetStringUTFLength(str);
//    const char *strChars = env->GetStringUTFChars(str, (jboolean *) 0);
//    std::string Result(strChars, len);
//    env->ReleaseStringUTFChars(str, strChars);
//    return Result;
//}

JNIEXPORT jstring JNICALL
Java_com_arophix_jniexample_MainActivity_stringFromJNI(JNIEnv *env, jobject thiz, jobject assetManagerFromJava){
    
    
    jstring stringValue = env->NewStringUTF("test");
    
    int foo1 [5] = { 16, 2, 77, 40, 12071 };
    int *foo2 = (int*)malloc(5);
    foo2[0] = 20;
    foo2[1] = 8;
    foo2[2] = 55;
    foo2[3] = 6;
    foo2[4] = 52;
    
    jclass stringClass = env->FindClass("java/lang/String");
    jmethodID getBytesMId = env->GetMethodID(stringClass, "getBytes", "()[B");
    
    jbyteArray keyBytes = (jbyteArray)env->CallObjectMethod(stringValue, getBytesMId);
    
    // determine the needed length and allocate a buffer for it
    jsize num_bytes = env->GetArrayLength(keyBytes);


    // obtain the array elements
    jbyte* elements = env->GetByteArrayElements(keyBytes, NULL);
    if (!elements) {
        // handle JNI error ...
    }
    
    for(int i = 0; i < num_bytes; i++) {
        char ch = elements[i];
        ALOGI("arrayLength: %c", ch);
    }
    
    AAssetManager *assetManager = AAssetManager_fromJava(env, assetManagerFromJava);
    if(assetManager != nullptr) {
        AAssetDir* assetDir = AAssetManager_openDir(assetManager, "");
        const char* filename = (const char*)NULL;
        while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) {
            AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);
            if (asset != nullptr) {
                ALOGI("Opened test file");
                char buf[1024];
                int nb_read = 0;
                while ((nb_read = AAsset_read(asset, buf, 27)) > 0) {
                    ALOGI("buf: %s", buf);
                }
    
                AAsset_close(asset);
            } else {
                ALOGI("Failed to open test file");
            }
        }
        AAssetDir_close(assetDir);
    }
    
    

    // Do not forget to release the element array provided by JNI:
    env->ReleaseByteArrayElements(keyBytes, elements, JNI_ABORT);
    
    jstring arg3 = env->NewStringUTF("test_key");
    
    const char *key_Name = env->GetStringUTFChars(arg3, 0);
    
//    //TODO : for testing
//    jstring jstr = env->NewStringUTF("Ñandú");  //can pass param also
//    std::string result = convertJStringToString(env, jstr);
    
//    __android_log_write
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
};

/**
 * This tow paths help JNI find the correct Java classes.
 */
static const char *classPathNameForAroMemory = "com/arophix/jniexample/jniobjects/AroMemory";
static const char *classPathNameForAroStorage = "com/arophix/jniexample/jniobjects/AroStorage";

/**
 * JNI methods. Note that all the JNI methods have the first parameters same.
 * The first one refers to the JNIEnv pointer, which enable you access to all the JNI methods,
 * while the second one refers to the Java instance itself.
 * The rest parameters correspond to the Java parameters sequentially.
 */
static jstring getNameFromNative(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF("native_arphix_memory_name_abc");
}

static jint getJniVersion(JNIEnv *env, jobject thiz) {
    return env->GetVersion();
}

/**
 * Throw an Exception from native by simply checking the length of fingerprint array.
 */
static bool validateDeviceFingerprint(JNIEnv *env,
                                      jobject thiz /*thiz is referring to AroMemory instance*/,
                                      jbyteArray deviceFingerprintData) {

    jint arrayLength = env->GetArrayLength(deviceFingerprintData);
    ALOGI("arrayLength: %d", arrayLength);
    if (arrayLength != 16) {

        jclass jcls = env->FindClass("java/lang/Exception");
        env->ThrowNew(jcls, "Invalid deviceFingerprint detected.");

        /**
         * The point worth mentioning here is that, on immediate encounter of a throw method,
         * control is *not* transferred to the Java code; instead, it waits until the return
         * statement is encountered. There can be lines of code in between the throw method
         * and return statement. Both the throw and JNI functions return zero on success
         * and a negative value otherwise.
         */

        return false;
    }

    return true;
}

static jstring computeStorageSignatureNative(JNIEnv *env,
                                             jobject thiz /*thiz is referring to AroStorage instance*/,
                                             jobject aroMemory,
                                             jbyteArray deviceFingerprintData) {

    /**
     * Example of handling exception, i.e. IllegalArgumentException, from native code
     */
    if (aroMemory == NULL) {
        jboolean flag = env->ExceptionCheck();
        if (flag) {
            env->ExceptionClear();
            /* code to handle exception */
        }
        jclass jcls = env->FindClass("java/lang/IllegalArgumentException");
        env->ThrowNew(jcls, "Argument cannot be null.");

        /**
         * The point worth mentioning here is that, on immediate encounter of a throw method,
         * control is *not* transferred to the Java code; instead, it waits until the return
         * statement is encountered. There can be lines of code in between the throw method
         * and return statement. Both the throw and JNI functions return zero on success
         * and a negative value otherwise.
         */

        return NULL;
    }

    if(!validateDeviceFingerprint(env, aroMemory, deviceFingerprintData)) {
        return NULL;
    }

    /** Example of accessing "jobject" of argument list **/
    // Find the AroStorage class
    jclass aroMemClazz = env->FindClass(classPathNameForAroMemory);
    // Get the methodID
    jmethodID jmethodGetId = env->GetMethodID(aroMemClazz, "getId", "()I");
    // Call int primitive method.
    jint idNo = env->CallIntMethod(aroMemory, jmethodGetId);
    ALOGI("idNo: %d", idNo);

    /** Example of calling "thiz" object method **/
    // Find the AroStorage class.
    jclass aroStorageClazz = g_aroStorageClazz;//env->FindClass(classPathNameForAroStorage);
    // Get the needed methodID.
    jmethodID getName = env->GetMethodID(aroStorageClazz, "getName", "()Ljava/lang/String;");
    // Call object method as String is an java object.
    jstring name = (jstring)env->CallObjectMethod(thiz, getName);
    // Get a const char * reference from jstring
    const char *nameUtf8 = env->GetStringUTFChars(name, 0);
    ALOGI("%s", nameUtf8);
    // Remember to release the const char *  pointer` if it is no longer needed.
    env->ReleaseStringUTFChars(name, nameUtf8);
    env->DeleteLocalRef(name);

    /** Example of accessing static field **/
    jfieldID staticField = env->GetStaticFieldID(aroStorageClazz, "sAroStroageDescriptor", "Ljava/lang/String;");
    jstring descriptor = (jstring)env->GetStaticObjectField(aroStorageClazz, staticField);
    // Get a const char * reference from jstring
    const char *nameUtf8ForDescriptor = env->GetStringUTFChars(descriptor ,0);
    ALOGI("%s", nameUtf8ForDescriptor);
    // Remember to release the const char *  pointer if it is no longer needed.
    env->ReleaseStringUTFChars(descriptor, nameUtf8ForDescriptor);
    env->DeleteLocalRef(descriptor);

    /** Example of accessing static method **/
    jmethodID staticMethod = env->GetStaticMethodID(aroStorageClazz, "getAroStroageDescriptor", "(IFLjava/lang/String;)Ljava/lang/String;");
    int arg1 = 100;
    float arg2 = 123.456;
    jstring arg3 = env->NewStringUTF("Native arg3");
    jstring descriptorFromStaticMethod = (jstring)env->CallStaticObjectMethod(aroStorageClazz, staticMethod, arg1, arg2, arg3);
    // Get a const char * reference from jstring
    const char *nameUtf8ForDescriptorFromStaticMethod = env->GetStringUTFChars(descriptorFromStaticMethod ,0);
    ALOGI("%s", nameUtf8ForDescriptorFromStaticMethod);
    // Remember to release the const char *  pointer if it is no longer needed.
    env->ReleaseStringUTFChars(descriptorFromStaticMethod, nameUtf8ForDescriptorFromStaticMethod);
    env->DeleteLocalRef(descriptorFromStaticMethod);
    env->DeleteLocalRef(arg3);

    /** Example of accessing byte array and using array region API **/
    jint arrayLength = env->GetArrayLength(deviceFingerprintData);
    ALOGI("arrayLength: %d", arrayLength);
    jbyte *jbyteBuffer = (jbyte*)malloc(8);
    memset(jbyteBuffer, 0x01, 8);
    env->SetByteArrayRegion(deviceFingerprintData, 0, 8, jbyteBuffer);
    // Get the starting pointer of byte array
    jbyte *byteArray1 = env->GetByteArrayElements(deviceFingerprintData, 0);
    for (int i = 0; i < arrayLength; ++i) {
        // Print the array to verify the value is updated to 0x01.
        ALOGI("array[%d]: 0x%02x, ", i, byteArray1[i]&0x000000ff);
    }
    // Free the allocated memory
    free(jbyteBuffer);
    // Release the local reference of byte array
    env->ReleaseByteArrayElements(deviceFingerprintData, byteArray1, 0);

    return env->NewStringUTF("AroStroage_SIGNATURE_12344321");
}

JNIEXPORT jint JNICALL
Java_com_arophix_jniexample_jniobjects_AroStorage_getJniVersion(JNIEnv *env, jobject instance) {
    return env->GetVersion();
}

/**
 * Definition of the native method struct is as below:
 typedef struct {
    const char* name; // the Java method name
    const char* signature; // the Java method signature
    void*       fnPtr; // the C function pointer.
 } JNINativeMethod;
 */
static JNINativeMethod aroMemoryMethods[] = {
        {"getNameFromNative", "()Ljava/lang/String;", (void*)getNameFromNative},
};

static JNINativeMethod aroStorageMethods[] = {
        {"getJniVersion", "()I", (void*)getJniVersion},
        {"computeStorageSignatureNative", "(Lcom/arophix/jniexample/jniobjects/AroMemory;[B)Ljava/lang/String;", (void*)computeStorageSignatureNative},
        {"downlaodImageNativeAsyncTask", "()I", (void*)downlaodImageNativeAsyncTask},
};


/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
                                 JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;
    clazz = env->FindClass(className);
    if (clazz == NULL) {
        ALOGE("Native registration unable to find class '%s'", className);
        return JNI_FALSE;
    }
    if (env->RegisterNatives(clazz, gMethods, numMethods) < 0) {
        ALOGE("RegisterNatives failed for '%s'", className);
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

/*
 * Register native methods for all classes we know about.
 *
 * returns JNI_TRUE on success.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env,
                               classPathNameForAroMemory,
                               aroMemoryMethods,
                               sizeof(aroMemoryMethods) / sizeof(aroMemoryMethods[0]))) {
        return JNI_FALSE;
    }

    if (!registerNativeMethods(env,
                               classPathNameForAroStorage,
                               aroStorageMethods,
                               sizeof(aroStorageMethods) / sizeof(aroStorageMethods[0]))) {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

/* this function is run by the start_backgroud_task thread */
void* download_image_func(void *arg)
{
    ALOGI("download_image started\n");

    ALOGI("threadFunction download_image_func is being attached.");
    JNIEnv *env = NULL;
    JavaVM *javaVM = g_jvm;
    jint res = javaVM->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (res != JNI_OK) {
        res = javaVM->AttachCurrentThread(&env, NULL);
        if (JNI_OK != res) {
            ALOGI("Failed to AttachCurrentThread, ErrorCode = %d", res);
            return NULL;
        }

        ALOGI("AttachCurrentThread succeed ...");
    }

    // Get the needed methodID.
    jmethodID getImageUrl = env->GetMethodID(g_aroStorageClazz, "getImageUrl", "()Ljava/lang/String;");
    // Call object method as String is an java object.
    jstring imageUrl = (jstring)env->CallObjectMethod((jobject)g_aroStorageObj, getImageUrl);
    // Get a const char * reference from jstring
    const char *imageUrlChars = env->GetStringUTFChars(imageUrl, 0);
    ALOGI("%s", imageUrlChars);
    // Remember to release the const char *  pointer` if it is no longer needed.
    env->ReleaseStringUTFChars(imageUrl, imageUrlChars);
    env->DeleteLocalRef(imageUrl);

    int counter = 0;
    /* sleep 100ms * 100 */
    while(++counter <= 100) {
        usleep(100);
    }

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
    }

    ALOGI("download_image finished\n");

    g_jvm->DetachCurrentThread();
    ALOGI("download_image thread is detached\n");

    /* the function must return something - NULL will do */
    return NULL;
}

/**
 * using linux pthread APIs to start a new background thread and trying to access JNI functions.
 *
 * 1. Get reference to the JVM environment context using GetEnv
 * 2. Attach the context if necessary using AttachCurrentThread
 * 3. call the JNI method as normal using CallObjectMethod
 * 4. Detach using DetachCurrentThread

 */
static void downlaodImageNativeAsyncTask(JNIEnv *env,
                                  jobject aroStorage /*thiz is referring to AroStorage instance*/)
{
    // Make a global reference to g_aroStorageObj for future usage.
    g_aroStorageObj = (jclass)g_uenv.env->NewGlobalRef(aroStorage);

    /* thread reference */
    pthread_t image_download_thread;

    /* create a thread which executes a image download task */
    if(pthread_create(&image_download_thread, NULL, download_image_func, aroStorage)) {
        fprintf(stderr, "Error creating thread\n");
    }
}

/*
 * The VM calls JNI_OnLoad when the native library is loaded (for example, through System.loadLibrary).
 * JNI_OnLoad must return the JNI version needed by the native library. In order to use any of the
 * new JNI functions, a native library must export a JNI_OnLoad function that returns JNI_VERSION_1_2.
 * If the native library does not export a JNI_OnLoad function, the VM assumes that the library only
 * requires JNI version JNI_VERSION_1_1. If the VM does not recognize the version number returned
 * by JNI_OnLoad, the native library cannot be loaded.
 */
jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    g_jvm = vm;

    g_uenv.venv = NULL;
    jint result = -1;
    JNIEnv* env = NULL;
    jclass aroStorageClazz = NULL;

    ALOGI("JNI_OnLoad started.");
    if (vm->GetEnv(&g_uenv.venv, JNI_VERSION_1_6) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed");
        goto bail;
    }
    env = g_uenv.env;

    // Find the AroStorage class.
    aroStorageClazz = g_uenv.env->FindClass(classPathNameForAroStorage);
    // Make a global reference to aroStorageClazz for future usage.
    g_aroStorageClazz = (jclass)g_uenv.env->NewGlobalRef(aroStorageClazz);

    if (registerNatives(env) != JNI_TRUE) {
        ALOGE("ERROR: registerNatives failed");
        goto bail;
    }

    result = JNI_VERSION_1_6;

    bail:

    ALOGI("JNI_OnLoad finished.");
    return result;
}

/**
 * The VM calls JNI_OnUnload when the class loader containing the native library is garbage collected.
 * This function can be used to perform cleanup operations. Because this function is called in an
 * unknown context (such as from a finalizer), the programmer should be conservative on using Java
 * VM services, and refrain from arbitrary Java call-backs. Note that JNI_OnLoad and JNI_OnUnload
 * are two functions optionally supplied by JNI libraries, not exported from the VM.
 */
void JNI_OnUnload(JavaVM *vm, void *reserved) {

    // Delete the global reference after use.
    g_uenv.env->DeleteGlobalRef(g_aroStorageObj);
    g_uenv.env->DeleteGlobalRef(g_aroStorageClazz);

    ALOGI("GlobalRef is deleted\n");
}


#ifdef __cplusplus
}
#endif