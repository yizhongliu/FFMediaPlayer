//
// Created by llm on 20-6-11.
//

#ifndef FFMEDIAPLAYER_RENDERVIDEOPAD_H
#define FFMEDIAPLAYER_RENDERVIDEOPAD_H

#include "FFPad.h"
#include "safe_queue.h"

class RenderVideoPad : public FFPad {
public:
    RenderVideoPad();
    virtual ~RenderVideoPad();
    virtual void addData(void* frame);
    virtual void* getData();
    virtual bool isDataBufferEmpty();

private:
    SafeQueue<AVFrame *> frames;
};

#endif //FFMEDIAPLAYER_RENDERVIDEOPAD_H
