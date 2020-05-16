//
// Created by llm on 20-5-11.
//

#ifndef FFMEDIAPLAYER_FFELEMENT_H
#define FFMEDIAPLAYER_FFELEMENT_H

#include <list>
#include "FFPad.h"

extern "C" {
#include <libavformat/avformat.h>
}

using namespace std;

//需要和MediaPlayerInterface的定义保持一直,用于消息回调
typedef void (*notify_callback_f)(int msg, int ext1, int ext2);

enum element_state {
    ELEMENT_STATE_OPEN = 0,
    ELEMENT_STATE_PLAYING,
    ELEMENT_STATE_PAUSE,
    ELEMENT_STATE_STOP
};

class FFElement {
public:
    virtual  ~FFElement() {
        list<FFPad*>::iterator it;

        for(it = pads.begin(); it != pads.end(); ) {
            FFPad* temp = *it;
            pads.erase(it++);
            delete temp;
        }

    }

    virtual int    open(AVFormatContext * avFormatContext, notify_callback_f notifyFunc) = 0;
    virtual int    start() = 0;
    virtual int    pause() = 0;
    virtual int    stop() = 0;
    virtual int    release() = 0;
    virtual int    reset() = 0;

    void  connectPads(FFPad* sourcePad, FFPad* sinkPad) {
        sourcePad->setObserver(sinkPad);
    }

    void addPad(FFPad* pad) {
        pads.push_back(pad);
    }

protected:
    notify_callback_f mNotify = 0;
    list<FFPad*> pads;

    bool isPlaying = false;
    element_state elementState;

};

#endif //FFMEDIAPLAYER_FFELEMENT_H
