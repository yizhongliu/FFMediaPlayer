//
// Created by llm on 20-6-11.
//

#ifndef FFMEDIAPLAYER_PARAMETER_H
#define FFMEDIAPLAYER_PARAMETER_H

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

};

typedef struct {
    AVFormatContext* formatContext;
    AVCodecContext *audioCodecContext;
    AVCodecContext *videoCodecContext;

    int videoIndex;
    int audioIndex;

} PLAYER_PARAMETERS;

#endif //FFMEDIAPLAYER_PARAMETER_H
