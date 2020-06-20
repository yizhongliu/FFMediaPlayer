//
// Created by llm on 20-5-15.
//

#include "FFPad.h"

extern "C" {
#include <libavformat/avformat.h>
}

FFPad::~FFPad() {

}

pad_type FFPad::getPadType() {
    return padType;
}

pad_media_type FFPad::getPadMediaType() {
    return mediaType;
}

bool FFPad::isReceiveData() {
    return receiveData;
}

void FFPad::setRecivieData(bool flag) {
    receiveData = flag;
}

