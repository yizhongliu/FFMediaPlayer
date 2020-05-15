//
// Created by llm on 20-5-15.
//

#ifndef FFMEDIAPLAYER_FRAMEPAD_H
#define FFMEDIAPLAYER_FRAMEPAD_H


#include "FFPad.h"

extern "C" {
#include <libavformat/avformat.h>
};

class DemuxVideoPad:public FFPad {
    DemuxVideoPad();
    virtual void update(void* frame);
};

#endif //FFMEDIAPLAYER_FRAMEPAD_H
