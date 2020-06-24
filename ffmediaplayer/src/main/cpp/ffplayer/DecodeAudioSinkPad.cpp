//
// Created by llm on 20-6-20.
//

#include "DecodeAudioSinkPad.h"
#include "FFLog.h"

extern "C" {
#include <libavutil/time.h>
}

DecodeAudioSinkPad::DecodeAudioSinkPad() {
    padType = PAD_SINK;
    mediaType = PAD_VIDEO;

    packets.setReleaseCallback(releaseAVPacket);
    packets.setWork(1);
}

DecodeAudioSinkPad::~DecodeAudioSinkPad() {
    ALOGE("DecodeAudioSinkPad::~DecodeAudioSinkPad()");
    packets.clear();
}


void DecodeAudioSinkPad::addData(void *frame) {
 //     ALOGE("DecodeAudioSinkPad::addData");
    while (packets.size() > 100 && isReceiveData()) {
        av_usleep(10 * 1000);
        continue;
    }

    packets.push((AVPacket *) frame);
}

void *DecodeAudioSinkPad::getData() {
   // ALOGE("DecodeAudioSinkPad::getData");
    AVPacket *packet = 0;
    if (packets.size() > 0) {
        packets.pop(packet);
    }
    return packet;
}

bool DecodeAudioSinkPad::isDataBufferEmpty() {
    if (packets.empty()) {
        return true;
    } else {
        return false;
    }
}

