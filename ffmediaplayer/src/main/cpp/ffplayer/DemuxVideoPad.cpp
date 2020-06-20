//
// Created by llm on 20-5-15.
//

#include <FFLog.h>
#include "DemuxVideoPad.h"

void DemuxVideoPad::addData(void *frame) {

}

void* DemuxVideoPad::getData() {
    return 0;
}

DemuxVideoPad::DemuxVideoPad() {
    padType = PAD_SOURCE;
    mediaType = PAD_VIDEO;
}

DemuxVideoPad::~DemuxVideoPad() {
    ALOGE("DemuxVideoPad::~DemuxVideoPad()");
}

bool DemuxVideoPad::isDataBufferEmpty() {
    return true;
}

