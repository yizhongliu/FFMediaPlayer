//
// Created by llm on 20-3-21.
//

#include <jni.h>
#include <assert.h>
#include <android/log.h>
#include <memory>
#include <mutex>
#include "MediaPlayerListener.h"
#include "FFMediaPlayer.h"
#include "macro.h"
#include "FFLog.h"

//#define LOG_TAG "FFMediaPlayer-JNI"
//#define ALOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
//#define ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
//#define ALOGW(...)  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
//#define ALOGD(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

using namespace std;

//指定类的路径，通过FindClass 方法来找到对应的类
const char* className  = "pri/tool/ffmediaplayer/MediaPlayer";

struct fields_t {
    jfieldID    context;     //将native层的player设置给java层
    jfieldID    surface_texture;

    jmethodID   post_event;
};

static fields_t fields;

static mutex sLock;

JavaVM *javaVM = NULL;

void jniThrowException(JNIEnv *env, const char *name, const char *msg)
{
    jclass cls = env->FindClass(name);
    if (cls != NULL) {
        env->ThrowNew(cls, msg);
    }
    env->DeleteLocalRef(cls);
}

//---------------------------------------------------------------------------------------------------------

// ref-counted object for callbacks
class JNIMediaPlayerListener: public MediaPlayerListener
{
public:
    JNIMediaPlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz);
    ~JNIMediaPlayerListener();
    virtual void notify(int msg, int ext1, int ext2);
private:
    JNIMediaPlayerListener();
    jclass      mClass;     // Reference to MediaPlayer class
    jobject     mObject;    // Weak ref to MediaPlayer Java object to call on
};

JNIMediaPlayerListener::JNIMediaPlayerListener(JNIEnv* env, jobject thiz, jobject weak_thiz)
{

    // Hold onto the MediaPlayer class for use in calling the static method
    // that posts events to the application thread.
    jclass clazz = env->GetObjectClass(thiz);
    if (clazz == NULL) {
        ALOGE("Can't find pri/tool/ffmediaplayer/MediaPlayer");
        jniThrowException(env, "java/lang/Exception", NULL);
        return;
    }

    mClass = (jclass)env->NewGlobalRef(clazz);

    // We use a weak reference so the MediaPlayer object can be garbage collected.
    // The reference is only used as a proxy for callbacks.
    mObject  = env->NewGlobalRef(weak_thiz);

    ALOGE("JNIMediaPlayerListener contruct");
}

JNIMediaPlayerListener::~JNIMediaPlayerListener()
{
    JNIEnv *env = NULL;

    int status = javaVM->GetEnv((void**)&env, JNI_VERSION_1_4);
    if (status < 0) {
        javaVM->AttachCurrentThread(&env, NULL);
        ALOGE("AttachCurrentThread");
    }
    // remove global referencesgi
    env->DeleteGlobalRef(mObject);
    env->DeleteGlobalRef(mClass);

    if (status < 0) {
        javaVM->DetachCurrentThread();
    }

    ALOGE("JNIMediaPlayerListener destruct");
}

void JNIMediaPlayerListener::notify(int msg, int ext1, int ext2) {

    JNIEnv *env = NULL;

    int status = javaVM->GetEnv((void**)&env, JNI_VERSION_1_4);
    if (status < 0) {
        javaVM->AttachCurrentThread(&env, NULL);
        ALOGE("AttachCurrentThread");
    }

    env->CallStaticVoidMethod(mClass, fields.post_event, mObject,
                              msg, ext1, ext2);

    if (env->ExceptionCheck()) {
        ALOGW("An exception occurred while notifying an event.");
        env->ExceptionClear();
    }

    if (status < 0) {
        javaVM->DetachCurrentThread();
        ALOGE("DetachCurrentThread");
    }

}
//---------------------------------------------------------------------------------------------------------


static FFMediaPlayer* getMediaPlayer(JNIEnv* env, jobject thiz)
{
    std::lock_guard<std::mutex> lock(sLock);
    FFMediaPlayer* const p = (FFMediaPlayer*)env->GetLongField(thiz, fields.context);
    return p;
}

static void setMediaPlayer(JNIEnv* env, jobject thiz, FFMediaPlayer* player)
{
    std::lock_guard<std::mutex> lock(sLock);
    env->SetLongField(thiz, fields.context, (jlong)player);
}
//---------------------------------------------------------------------------------------------------------------



static void pri_tool_MediaPlayer_native_init(JNIEnv *env) {
    jclass clazz;

    clazz = env->FindClass(className);
    if (clazz == NULL) {
        return;
    }

    fields.context = env->GetFieldID(clazz, "mNativeContext", "J");
    if (fields.context == NULL) {
        return;
    }

    fields.post_event = env->GetStaticMethodID(clazz, "postEventFromNative",
                                               "(Ljava/lang/Object;III)V");
    if (fields.post_event == NULL) {
        return;
    }

    fields.surface_texture = env->GetFieldID(clazz, "mNativeSurfaceTexture", "J");
    if (fields.surface_texture == NULL) {
        return;
    }

    env->DeleteLocalRef(clazz);
}

static void pri_tool_MediaPlayer_native_setup(JNIEnv *env, jobject thiz, jobject weak_this) {
    ALOGI("native_setup");

    //TODO:没有用智能指针，需要主动delete
    //c++11 的share_ptr 和Android sp指针有一点差别，无法手动增加计数
    FFMediaPlayer *mp = new FFMediaPlayer();
    if (mp == NULL) {
        jniThrowException(env, "java/lang/RuntimeException", "Out of memory");
        return;
    }

    // create new listener and give it to MediaPlayer
    shared_ptr<JNIMediaPlayerListener> listener(new JNIMediaPlayerListener(env, thiz, weak_this));
    shared_ptr<MediaPlayerListener> baseListener = dynamic_pointer_cast<JNIMediaPlayerListener>(listener);
    mp->setListener(baseListener);

    //将FFMediaPlayer指针地址保存到java层对应的对象实例实例中
    // Stow our new C++ MediaPlayer in an opaque field in the Java object.
    setMediaPlayer(env, thiz, mp);
}

static void pri_tool_MediaPlayer_native_setDataSource(JNIEnv *env, jobject thiz, jstring path) {
    ALOGI("native_setDataSource");

    FFMediaPlayer *mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    //将fileDescriptor 转化为native层能识别的文件fd
    if (path == NULL) {
        jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        return;
    }

    const char *dataSource = env->GetStringUTFChars(path, 0);

    int ret = mp->setDataSource(const_cast<char *>(dataSource));
    if (ret != STATUS_OK) {
        jniThrowException(env, "java/io/IOException", NULL);
    }

    env->ReleaseStringUTFChars(path, dataSource);
}

static void pri_tool_MediaPlayer_native_prepareAsync(JNIEnv *env, jobject thiz) {
    ALOGI("native_prepareAsync");

    FFMediaPlayer *mp = getMediaPlayer(env, thiz);
    if (mp == NULL) {
        jniThrowException(env, "java/lang/IllegalStateException", NULL);
        return;
    }

    int ret = mp->prepareAsync();
    if (ret != STATUS_OK) {
        jniThrowException(env, "java/io/IOException", NULL);
    }

}

static void pri_tool_MediaPlayer_native_testCallback(JNIEnv *env, jobject thiz, jboolean bNewThread) {
    FFMediaPlayer *mp = getMediaPlayer(env, thiz);
    mp->testCallback(bNewThread);


    if (bNewThread) {
   //     mp->setListener(0);
    }
}


static JNINativeMethod gMethods[] = {
        {"native_init",         "()V",                              (void *)pri_tool_MediaPlayer_native_init},
        {"native_setup",        "(Ljava/lang/Object;)V",            (void *)pri_tool_MediaPlayer_native_setup},
        {"native_setDataSource",        "(Ljava/lang/String;)V",            (void *)pri_tool_MediaPlayer_native_setDataSource},
        {"native_prepareAsync",        "()V",            (void *)pri_tool_MediaPlayer_native_prepareAsync},
        {"native_testCallback",        "(Z)V",            (void *)pri_tool_MediaPlayer_native_testCallback},
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

    javaVM = vm;

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