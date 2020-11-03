//
// Created by llm on 20-7-17.
//

#ifndef FFMEDIAPLAYER_SINKPACKETPAD_H
#define FFMEDIAPLAYER_SINKPACKETPAD_H

#include "FFPad.h"
#include "safe_queue.h"

class SinkPacketPad : public FFPad {
public:
    SinkPacketPad(pad_type padType, pad_media_type mediaType);
    virtual ~SinkPacketPad();
    virtual void addData(void* frame);
    virtual void* getData();
    virtual bool isDataBufferEmpty();

private:
    SafeQueue<AVPacket *> packets;
};

#endif //FFMEDIAPLAYER_SINKPACKETPAD_H
