//
// Created by llm on 20-5-15.
//

#ifndef FFMEDIAPLAYER_FRAMEPAD_H
#define FFMEDIAPLAYER_FRAMEPAD_H


#include "FFPad.h"

class DemuxVideoPad:public FFPad {
public:
    DemuxVideoPad();
    virtual void addData(void* frame);
    virtual void* getData();
};

#endif //FFMEDIAPLAYER_FRAMEPAD_H
