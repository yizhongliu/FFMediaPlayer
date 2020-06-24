//
// Created by llm on 20-6-22.
//

#ifndef FFMEDIAPLAYER_SOURCEPAD_H
#define FFMEDIAPLAYER_SOURCEPAD_H

#include "FFPad.h"

class SourcePad:public FFPad {
public:
    SourcePad(pad_type padType, pad_media_type mediaType);
    virtual void addData(void* frame);
    virtual void* getData();
    virtual bool isDataBufferEmpty();
};

#endif //FFMEDIAPLAYER_SOURCEPAD_H
