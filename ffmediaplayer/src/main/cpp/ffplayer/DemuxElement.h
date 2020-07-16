//
// Created by llm on 20-5-11.
//

#ifndef FFMEDIAPLAYER_DEMUXELEMENT_H
#define FFMEDIAPLAYER_DEMUXELEMENT_H

#include "FFElement.h"

class DemuxElement: public FFElement {
public:
    DemuxElement(const char* filepath);
    ~DemuxElement();

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
    char* filePath = 0;
    PLAYER_PARAMETERS* avContext;

    FFPad* audioPad = 0;
    FFPad* videoPad = 0;

    pthread_t pid_start;

};

#endif //FFMEDIAPLAYER_DEMUXELEMENT_H
