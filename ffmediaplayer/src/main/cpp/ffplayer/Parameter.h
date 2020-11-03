//
// Created by llm on 20-6-11.
//

#ifndef FFMEDIAPLAYER_PARAMETER_H
#define FFMEDIAPLAYER_PARAMETER_H

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"

};

typedef enum {
    TIME_BASE_AUDIO,
    TIME_BASE_VIDEO,
    TIME_BASE_OTHER
} TIME_BASE_TYPE;

typedef struct {
    AVFormatContext* formatContext;
    AVCodecContext *audioCodecContext;  //音频解码上下文
    AVCodecContext *videoCodecContext;  //视频解码上下文

    int videoIndex; //视频流的id
    int audioIndex; //音频流的id

    TIME_BASE_TYPE timeBaseType;

    double streamTime; //时间基准

} PLAYER_PARAMETERS;


#endif //FFMEDIAPLAYER_PARAMETER_H
