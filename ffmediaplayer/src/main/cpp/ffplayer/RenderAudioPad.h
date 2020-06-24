//
// Created by llm on 20-6-23.
//

#ifndef FFMEDIAPLAYER_RENDERAUDIOPAD_H
#define FFMEDIAPLAYER_RENDERAUDIOPAD_H

#include "FFPad.h"
#include "safe_queue.h"

class RenderAudioPad : public FFPad {
public:
    RenderAudioPad();
    virtual ~RenderAudioPad();
    virtual void addData(void* frame);
    virtual void* getData();
    virtual bool isDataBufferEmpty();

private:
    SafeQueue<AVFrame *> frames;
};

#endif //FFMEDIAPLAYER_RENDERAUDIOPAD_H
