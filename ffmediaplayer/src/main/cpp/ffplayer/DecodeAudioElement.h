//
// Created by llm on 20-6-20.
//

#ifndef FFMEDIAPLAYER_DECODEAUDIOELEMENT_H
#define FFMEDIAPLAYER_DECODEAUDIOELEMENT_H


#include <jni.h>
#include "FFElement.h"

class DecodeAudioElement: public FFElement {
public:
    DecodeAudioElement();
    ~DecodeAudioElement();

    virtual int    open(PLAYER_PARAMETERS &avContext, notify_callback_f notifyFunc);
    virtual int    start();
    virtual int    pause();
    virtual int    stop();
    virtual int    release();
    virtual int    reset();
    virtual int    setSurface(ANativeWindow* window);
    virtual bool   isDataBufferEmpty();


    void addPad(FFPad* pad);
    void _start();

private:

    pthread_t pid_start;

    FFPad* sinkPad = 0;
    FFPad* sourcePad = 0;

    AVCodecContext *codecContext = 0;
};

#endif //FFMEDIAPLAYER_DECODEAUDIOELEMENT_H
