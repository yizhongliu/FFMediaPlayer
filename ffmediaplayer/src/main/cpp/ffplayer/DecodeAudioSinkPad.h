//
// Created by llm on 20-6-20.
//

#ifndef FFMEDIAPLAYER_DECODEAUDIOSINKPAD_H
#define FFMEDIAPLAYER_DECODEAUDIOSINKPAD_H

#include "FFPad.h"
#include "safe_queue.h"

extern "C" {
#include "libavcodec/avcodec.h"
};


class DecodeAudioSinkPad: public FFPad {
public:
    DecodeAudioSinkPad();
    virtual ~DecodeAudioSinkPad();
    virtual void addData(void* frame);
    virtual void* getData();
    virtual bool isDataBufferEmpty();

private:
    SafeQueue<AVPacket *> packets;
};

#endif //FFMEDIAPLAYER_DECODEAUDIOSINKPAD_H
