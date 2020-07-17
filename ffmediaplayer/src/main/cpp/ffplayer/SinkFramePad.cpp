//
// Created by llm on 20-7-17.
//
#include "SinkFramePad.h"
#include "FFLog.h"


SinkFramePad::SinkFramePad(pad_type padType, pad_media_type mediaType) : FFPad(padType, mediaType) {

    ALOGE("SinkFramePad::SinkFramePad(%d, %d)", padType, mediaType);

    frames.setReleaseCallback(releaseAVFrame);
    frames.setWork(1);
}


SinkFramePad::~SinkFramePad() {
    ALOGE("SinkPad<T>::~SinkPad()");
    frames.clear();
}


void SinkFramePad::addData(void *frame) {
    while (frames.size() > 100  && isReceiveData()) {
        av_usleep(10 * 1000);
        continue;
    }

    frames.push((AVFrame *) frame);
}


void *SinkFramePad::getData() {
    AVFrame *frame = 0;
    if (frames.size() > 0) {
        frames.pop(frame);
    }
    return frame;
}


bool SinkFramePad::isDataBufferEmpty() {
    if (frames.empty()) {
        return true;
    } else {
        return false;
    }
}

