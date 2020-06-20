#ifndef FFMEDIAPLAYER_FFLOG_H
#define FFMEDIAPLAYER_FFLOG_H
#include <android/log.h>

//定义日志打印宏函数
#define LOG_TAG "FFMediaPlayer"
#define ALOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define ALOGW(...)  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGD(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)


#endif //FFMEDIAPLAYER_MACRO_H