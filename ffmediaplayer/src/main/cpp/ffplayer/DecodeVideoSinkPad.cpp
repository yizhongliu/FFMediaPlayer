//
// Created by llm on 20-5-16.
//


#include "DecodeVideoSinkPad.h"

DecodeVideoSinkPad::DecodeVideoSinkPad() {
    padType = PAD_SINK;
    mediaType = PAD_VIDEO;

    packets.setReleaseCallback(releaseAVPacket);
}

DecodeVideoSinkPad::~DecodeVideoSinkPad() {
    ALOGE("DecodeVideoSinkPad::~DecodeVideoSinkPad()");
    packets.clear();
}


void DecodeVideoSinkPad::addData(void *frame) {
  //  ALOGE("DecodeVideoSinkPad::addData");
    while (packets.size() > 100 && isReceiveData()) {
        av_usleep(10 * 1000);
        continue;
    }

    packets.push((AVPacket *) frame);
}

void *DecodeVideoSinkPad::getData() {
    AVPacket *packet = 0;
    if (packets.size() > 0) {
        packets.pop(packet);
    }
    return packet;
}

bool DecodeVideoSinkPad::isDataBufferEmpty() {
    if (packets.empty()) {
        return true;
    } else {
        return false;
    }
}
