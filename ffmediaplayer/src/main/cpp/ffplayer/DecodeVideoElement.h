//
// Created by llm on 20-5-14.
//

#ifndef FFMEDIAPLAYER_DECODEELEMENT_H
#define FFMEDIAPLAYER_DECODEELEMENT_H

#include "FFElement.h"

class DecodeVideoElement: public FFElement {
public:
    DecodeVideoElement(const char* filepath);
    ~DecodeVideoElement();

    virtual int    open(AVFormatContext * avFormatContext, notify_callback_f notifyFunc);
    virtual int    start();
    virtual int    pause();
    virtual int    stop();
    virtual int    release();
    virtual int    reset();

    static void receiveData(void* data);
private:
};

#endif //FFMEDIAPLAYER_DECODEELEMENT_H
