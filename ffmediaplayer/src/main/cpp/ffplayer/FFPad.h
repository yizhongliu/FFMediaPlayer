//
// Created by llm on 20-5-15.
//

#ifndef FFMEDIAPLAYER_PAD_H
#define FFMEDIAPLAYER_PAD_H

#include "IObserver.h"

enum pad_type {
    PAD_SOURCE = 0,
    PAD_SINK
};

enum pad_media_type {
    PAD_VIDEO = 0,
    PAD_AUDIO
};

typedef void (*chain_func)(void* data);


class FFPad : public IObserver{
public:
    virtual ~FFPad();

    pad_type getPadType();

    pad_media_type getPadMediaType();

    /*当buffer满了后, 函数会阻塞*/
    virtual void addData(void* data) = 0;

    virtual void* getData() = 0;

protected:
    pad_type padType;
    pad_media_type mediaType;
};

#endif //FFMEDIAPLAYER_PAD_H
