//
// Created by llm on 20-6-11.
//

#include "RenderVideoPad.h"

RenderVideoPad::RenderVideoPad() {
    padType = PAD_SINK;
    mediaType = PAD_VIDEO;

    frames.setReleaseCallback(releaseAVFrame);
}

void RenderVideoPad::addData(void *frame) {
    while (frames.size() > 100) {
        av_usleep(10 * 1000);
        continue;
    }

    frames.push((AVFrame *) frame);
}

RenderVideoPad::~RenderVideoPad() {
    ALOGE("RenderVideoPad::~RenderVideoPad()");
    frames.clear();
}

void *RenderVideoPad::getData() {
    AVFrame *frame = 0;
    if (frames.size() > 0) {
        frames.pop(frame);
    }
    return frame;
}

bool RenderVideoPad::isDataBufferEmpty() {
    if (frames.empty()) {
        return true;
    } else {
        return false;
    }
}

