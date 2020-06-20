//
// Created by llm on 20-5-16.
//

#ifndef FFMEDIAPLAYER_DECODEVIDEOSINKPAD_H
#define FFMEDIAPLAYER_DECODEVIDEOSINKPAD_H

#include "FFPad.h"
#include "../include/libavcodec/avcodec.h"
#include "safe_queue.h"

extern "C" {
#include "libavcodec/avcodec.h"
};


class DecodeVideoSinkPad: public FFPad {
public:
    DecodeVideoSinkPad();
    virtual ~DecodeVideoSinkPad();
    virtual void addData(void* frame);
    virtual void* getData();
    virtual bool isDataBufferEmpty();

private:
    SafeQueue<AVPacket *> packets;
};

#endif //FFMEDIAPLAYER_DECODEVIDEOSINKPAD_H
