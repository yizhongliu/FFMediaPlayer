//
// Created by llm on 20-6-22.
//

#include "SourcePad.h"

void SourcePad::addData(void *frame) {

}

void* SourcePad::getData() {
    return 0;
}

bool SourcePad::isDataBufferEmpty() {
    return true;
}

SourcePad::SourcePad(pad_type padType, pad_media_type mediaType) : FFPad(padType, mediaType) {

}

