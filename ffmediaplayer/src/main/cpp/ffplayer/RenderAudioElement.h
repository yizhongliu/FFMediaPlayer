//
// Created by llm on 20-6-22.
//

#ifndef FFMEDIAPLAYER_RENDERAUDIOELEMENT_H
#define FFMEDIAPLAYER_RENDERAUDIOELEMENT_H

#include <pthread.h>
#include <jni.h>
#include "FFElement.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
}


class RenderAudioElement: public FFElement {
public:
    RenderAudioElement();
    ~RenderAudioElement();

    virtual int    open(PLAYER_PARAMETERS &avContext, notify_callback_f notifyFunc);
    virtual int    start();
    virtual int    pause();
    virtual int    stop();
    virtual int    release();
    virtual int    reset();
    virtual bool   isDataBufferEmpty();

    void addPad(FFPad* pad);
    void _start();

    int setSurface(ANativeWindow* window);

    int getPCM();

    uint8_t *outBuffers = 0;
    int outChannels;
    int outSampleSize;
    int outSampleRate;
    int outBuffersSize;


private:
    FFPad* audioPad = 0;

    pthread_t pid_start;

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

    PLAYER_PARAMETERS avContext;

    SwrContext *swrContext = 0;

    //引擎
    SLObjectItf engineObject = 0;
    //引擎接口
    SLEngineItf engineInterface = 0;
    //混音器
    SLObjectItf outputMixObject = 0;
    //播放器
    SLObjectItf bqPlayerObject = 0;
    //播放器接口
    SLPlayItf bqPlayerPlay = 0;
    //播放器队列接口
    SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue = 0;

};

#endif //FFMEDIAPLAYER_RENDERAUDIOELEMENT_H
