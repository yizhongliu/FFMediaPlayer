//
// Created by llm on 20-6-9.
//

#ifndef FFMEDIAPLAYER_DECODEVIDEOSOURCEPAD_H
#define FFMEDIAPLAYER_DECODEVIDEOSOURCEPAD_H


#include "FFPad.h"

class DecodeVideoSourcePad: public FFPad {
public:
    DecodeVideoSourcePad();
    virtual ~DecodeVideoSourcePad();
    virtual void addData(void* frame);
    virtual void* getData();
    virtual bool isDataBufferEmpty();

private:
};

#endif //FFMEDIAPLAYER_DECODEVIDEOSOURCEPAD_H
