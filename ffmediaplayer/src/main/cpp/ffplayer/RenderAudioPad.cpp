//
// Created by llm on 20-6-23.
//

#include "RenderAudioPad.h"
#include "FFLog.h"

RenderAudioPad::RenderAudioPad() {
    padType = PAD_SINK;
    mediaType = PAD_AUDIO;

    frames.setReleaseCallback(releaseAVFrame);
    frames.setWork(1);
}

void RenderAudioPad::addData(void *frame) {
    ALOGE("RenderAudioPad::addData");
    while (frames.size() > 100) {
        av_usleep(10 * 1000);
        continue;
    }

    frames.push((AVFrame *) frame);
}

RenderAudioPad::~RenderAudioPad() {
    ALOGE("RenderVideoPad::~RenderVideoPad()");
    frames.clear();
}

void *RenderAudioPad::getData() {
    ALOGE("RenderAudioPad::getData");
    AVFrame *frame = 0;
    if (frames.size() > 0) {
        frames.pop(frame);
    }
    return frame;
}

bool RenderAudioPad::isDataBufferEmpty() {
    if (frames.empty()) {
        return true;
    } else {
        return false;
    }
}
