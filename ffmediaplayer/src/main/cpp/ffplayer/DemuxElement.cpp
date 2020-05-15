//
// Created by llm on 20-5-11.
//
#include "DemuxElement.h"
#include "../FFLog.h"
#include "../macro.h"

DemuxElement::DemuxElement(const char *filepath) {
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);
}

DemuxElement::~DemuxElement() {
    if (filePath != 0) {
        delete filePath;
        filePath = 0;
    }

    if (mNotify != 0) {
        mNotify = 0;
    }
}

int DemuxElement::open(AVFormatContext *avFormatContext, notify_callback_f notifyFunc) {
    ALOGI("enter %s", __FUNCTION__);
    mNotify = notifyFunc;

    //打开一个输入流并解析Header
    //第3个参数是input format. 传0是自动检测
    //第4个参数是dictionary,
    int ret = avformat_open_input(&avFormatContext, filePath, 0, 0);
    if (ret != 0) {
        ALOGE("avformat open file fail!!!  %s",  av_err2str(ret));
        mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, OPEN_URL_FAIL);
        return OPEN_URL_FAIL;
    }

    ret = avformat_find_stream_info(avFormatContext, 0);
    if (ret < 0) {
        ALOGE("avformat_find_stream_info fail：%s", av_err2str(ret));
        mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, FIND_STREAM_FAIL);
        return FIND_STREAM_FAIL;
    }

    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        int codecType = avFormatContext->streams[i]->codecpar->codec_type;
        switch (codecType) {
            case AVMEDIA_TYPE_VIDEO:
                videoIndex = i;
                break;
            case AVMEDIA_TYPE_AUDIO:
                audioIndex = i;
                break;
        }
    }

    return STATUS_OK;
}

int DemuxElement::start() {
    return 0;
}

int DemuxElement::pause() {
    return 0;
}

int DemuxElement::stop() {
    return 0;
}

int DemuxElement::release() {
    return 0;
}

int DemuxElement::reset() {
    return 0;
}

void DemuxElement::connectPads(FFPad *sourcePad, FFPad *sinkPad) {
    sourcePad->setObserver(sinkPad);
}




