//
// Created by llm on 20-3-21.
//

#include <jni.h>
#include <assert.h>
#include <android/log.h>

#define LOG_TAG "FFMediaPlayer-JNI"
#define ALOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGD(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct fields_t {
    jfieldID    context;
    jfieldID    surface_texture;

    jmethodID   post_event;
};
static fields_t fields;

static void pri_tool_MediaPlayer_native_init(JNIEnv *env) {

}

static void pri_tool_MediaPlayer_native_setup(JNIEnv *env, jobject thiz, jobject weak_this) {

}
static JNINativeMethod gMethods[] = {
        {"native_init",         "()V",                              (void *)pri_tool_MediaPlayer_native_init},
        {"native_setup",        "(Ljava/lang/Object;)V",            (void *)pri_tool_MediaPlayer_native_setup},
};

static int registerNativeMethods(JNIEnv* env, const char* className, JNINativeMethod* gMethods, int methodsNum) {
    jclass clazz;
    //找到声明native方法的类
    clazz = env->FindClass(className);
    if(clazz == NULL){
        return JNI_FALSE;
    }
    //注册函数 参数：java类 所要注册的函数数组 注册函数的个数
    if(env->RegisterNatives(clazz, gMethods, methodsNum) < 0){
        return JNI_FALSE;
    }
    return JNI_TRUE;
}

static int register_native_MediaPlayer(JNIEnv* env){
    //指定类的路径，通过FindClass 方法来找到对应的类
    const char* className  = "pri/tool/ffmediaplayer/MediaPlayer";
    return registerNativeMethods(env, className, gMethods, sizeof(gMethods)/ sizeof(gMethods[0]));
}

jint JNI_OnLoad(JavaVM* vm, void* /* reserved */)
{
    JNIEnv* env = NULL;
    jint result = -1;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        ALOGE("ERROR: GetEnv failed\n");
        goto bail;
    }
    assert(env != NULL);

    if (register_native_MediaPlayer(env) < 0) {
        ALOGE("ERROR: MediaPlayer native registration failed");
        goto bail;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;

    bail:
    return result;
}