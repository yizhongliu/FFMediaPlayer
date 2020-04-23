//
// Created by llm on 20-4-22.
//

#include "FFplayer.h"
#include "../macro.h"

FFplayer::FFplayer() {
    ALOGE("FFplayer contruct");
}

FFplayer::~FFplayer() {
    ALOGE("FFplayer decontruct");
}

int FFplayer::setDataSource(int fd, int64_t offset, int64_t length) {
    return 0;
}

int FFplayer::prepareAsync() {
    return 0;
}

int FFplayer::start() {
    return 0;
}

int FFplayer::stop() {
    return 0;
}

int FFplayer::pause() {
    return 0;
}

int FFplayer::reset() {
    return 0;
}

int FFplayer::release() {
    return 0;
}
