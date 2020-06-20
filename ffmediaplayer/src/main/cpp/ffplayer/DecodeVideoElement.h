//
// Created by llm on 20-5-14.
//

#ifndef FFMEDIAPLAYER_DECODEELEMENT_H
#define FFMEDIAPLAYER_DECODEELEMENT_H

#include <jni.h>
#include "FFElement.h"

class DecodeVideoElement: public FFElement {
public:
    DecodeVideoElement();
    ~DecodeVideoElement();

    virtual int    open(PLAYER_PARAMETERS &avContext, notify_callback_f notifyFunc);
    virtual int    start();
    virtual int    pause();
    virtual int    stop();
    virtual int    release();
    virtual int    reset();
    virtual int    setSurface(ANativeWindow* window);
    virtual bool   isDataBufferEmpty();

    static void receiveData(void* data);

    virtual void    connectPads(FFPad* sourcePad, FFPad* sinkPad);

    void addPad(FFPad* pad);
    void _start();

    int setSurface(JNIEnv* env, jobject surface);
private:

    pthread_t pid_start;

    FFPad* sinkPad = 0;
    FFPad* sourcePad = 0;

    AVCodecContext *codecContext = 0;
};

#endif //FFMEDIAPLAYER_DECODEELEMENT_H
