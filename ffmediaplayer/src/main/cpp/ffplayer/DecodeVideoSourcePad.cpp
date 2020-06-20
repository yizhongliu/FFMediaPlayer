//
// Created by llm on 20-6-10.
//

#include <FFLog.h>
#include "DecodeVideoSourcePad.h"

void DecodeVideoSourcePad::addData(void *frame) {
}

void* DecodeVideoSourcePad::getData() {
    return 0;
}

DecodeVideoSourcePad::DecodeVideoSourcePad() {
    padType = PAD_SOURCE;
    mediaType = PAD_VIDEO;
}

DecodeVideoSourcePad::~DecodeVideoSourcePad() {
    ALOGE("DecodeVideoSourcePad::~DecodeVideoSourcePad() ");
}

bool DecodeVideoSourcePad::isDataBufferEmpty() {
    return true;
}


