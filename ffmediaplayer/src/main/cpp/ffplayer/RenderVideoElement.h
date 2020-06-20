//
// Created by llm on 20-6-10.
//

#ifndef FFMEDIAPLAYER_RENDERVIDEOELEMENT_H
#define FFMEDIAPLAYER_RENDERVIDEOELEMENT_H

#include <pthread.h>
#include <android/native_window.h>
#include <jni.h>
#include "FFElement.h"

extern "C" {
#include <libavutil/time.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

typedef void (*RenderCallback)(uint8_t *, int, int, int);

class RenderVideoElement: public FFElement {
public:
    RenderVideoElement();
    ~RenderVideoElement();

    virtual int    open(PLAYER_PARAMETERS &avContext, notify_callback_f notifyFunc);
    virtual int    start();
    virtual int    pause();
    virtual int    stop();
    virtual int    release();
    virtual int    reset();
    virtual bool isDataBufferEmpty();

    void addPad(FFPad* pad);
    void _start();

    int setSurface(ANativeWindow* window);
    void renderFrame(uint8_t *src_data, int src_lineSize, int width, int height);


private:
    FFPad* videoPad = 0;

    pthread_t pid_start;


    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    ANativeWindow *window = 0;

    uint8_t *dst_data[4];
    int dst_linesize[4];
    SwsContext *sws_ctx = 0;
    int width;
    int height;

    RenderCallback renderCallback;
};

#endif //FFMEDIAPLAYER_RENDERVIDEOELEMENT_H
