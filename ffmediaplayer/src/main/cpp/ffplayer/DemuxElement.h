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

    virtual int    open(AVFormatContext * avFormatContext, notify_callback_f notifyFunc);
    virtual int    start();
    virtual int    pause();
    virtual int    stop();
    virtual int    release();
    virtual int    reset();
    virtual void    connectPads(FFPad* sourcePad, FFPad* sinkPad);

    void addPad(FFPad* pad);
    void _start();


private:
    char* filePath = 0;
    int videoIndex = -1;
    int audioIndex = -1;

    FFPad* audioPad = 0;
    FFPad* videoPad = 0;

    pthread_t pid_start;

    AVFormatContext* mAvFormatContext = 0;
};

#endif //FFMEDIAPLAYER_DEMUXELEMENT_H
