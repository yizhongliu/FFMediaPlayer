//
// Created by llm on 20-7-17.
//
#include "SinkPacketPad.h"
#include "FFLog.h"

SinkPacketPad::SinkPacketPad(pad_type padType, pad_media_type mediaType) : FFPad(padType, mediaType) {

    ALOGE("SinkPacketPad::SinkPacketPad(%d, %d)", padType, mediaType);
    packets.setReleaseCallback(releaseAVPacket);
    packets.setWork(1);
}


SinkPacketPad::~SinkPacketPad() {
    ALOGE("SinkPad<T>::~SinkPad()");
    packets.clear();
}


void SinkPacketPad::addData(void *frame) {
    while (packets.size() > 100  && isReceiveData()) {
        av_usleep(10 * 1000);
        continue;
    }

    packets.push((AVPacket *) frame);
}


void* SinkPacketPad::getData() {
    AVPacket *packet = 0;
    if (packets.size() > 0) {
        packets.pop(packet);
    }
    return packet;
}


bool SinkPacketPad::isDataBufferEmpty() {
    if (packets.empty()) {
        return true;
    } else {
        return false;
    }
}

