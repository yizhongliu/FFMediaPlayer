//
// Created by llm on 20-5-11.
//

#ifndef FFMEDIAPLAYER_FFELEMENT_H
#define FFMEDIAPLAYER_FFELEMENT_H

#include <list>
#include <android/native_window.h>
#include "FFPad.h"
#include "Parameter.h"

extern "C" {
#include <libavformat/avformat.h>
}

using namespace std;

//需要和MediaPlayerInterface的定义保持一直,用于消息回调
typedef void (*notify_callback_f)(int msg, int ext1, int ext2);

//通过回调 Surface接口向Surface填充数据显示
typedef void (*RenderCallback)(uint8_t *, int, int, int);

enum element_state {
    ELEMENT_STATE_OPEN = 0,
    ELEMENT_STATE_PLAYING,
    ELEMENT_STATE_PAUSE,
    ELEMENT_STATE_STOP
};

enum element_type {
    SOURCE = 0,
    DEMUX,
    VIDEO_DECODER,
    AUDIO_DECODER,
    FILTER,
    VIDEO_SINK,
    AUDIO_SINK
};

class FFElement {
public:
    virtual  ~FFElement() {
//        list<FFPad*>::iterator it;
//
//        for(it = pads.begin(); it != pads.end(); ) {
//            FFPad* temp = *it;
//            pads.erase(it++);
//            delete temp;
//        }

    }

    virtual int    open(PLAYER_PARAMETERS &avContext, notify_callback_f notifyFunc) = 0;
    virtual int    start() = 0;
    virtual int    pause() = 0;
    virtual int    stop() = 0;
    virtual int    release() = 0;
    virtual int    reset() = 0;
    virtual int    setSurface(ANativeWindow* window) = 0;
    virtual bool   isDataBufferEmpty() = 0;

    void  connectPads(FFPad* sourcePad, FFPad* sinkPad) {
        sourcePad->setObserver(sinkPad);
    }

    virtual void addPad(FFPad* pad) = 0;

    element_type getElementType() {
        return elementType;
    }

    void setReachEOF() {
        isPlaying = false;
    }


protected:
    notify_callback_f mNotify = 0;
//    list<FFPad*> pads;

    bool isPlaying = false;
    element_state elementState;
    element_type  elementType;

};

#endif //FFMEDIAPLAYER_FFELEMENT_H
