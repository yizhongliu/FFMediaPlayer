//
// Created by llm on 20-5-11.
//
#include <pthread.h>
#include "DemuxElement.h"
#include "FFLog.h"
#include "macro.h"

void *task_start(void *args) {
    ALOGE("enter: %s", __PRETTY_FUNCTION__);
    DemuxElement *element = static_cast<DemuxElement *>(args);
    element->_start();
    return 0;//一定一定一定要返回0！！！
}

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
    mAvFormatContext = avFormatContext;

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


    if (videoIndex == -1 && audioIndex == -1) {
        ALOGE("stream don't contain video or audio");
        mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, FIND_STREAM_FAIL);
        return FIND_STREAM_FAIL;
    }

    elementState = ELEMENT_STATE_OPEN;

    return STATUS_OK;
}

int DemuxElement::start() {
    ALOGE("enter: %s", __PRETTY_FUNCTION__);

    if (isPlaying == true) {
        ALOGE("enter: %s player has already started", __FUNCTION__);
        return MEDIA_ALREAD_START;
    }
    isPlaying = true;

    if (elementState != ELEMENT_STATE_PAUSE) {
        pthread_create(&pid_start, 0, task_start, this);
    }
    elementState = ELEMENT_STATE_PLAYING;

    return STATUS_OK;
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

void DemuxElement::addPad(FFPad* pad) {
    pads.push_back(pad);

    if (pad->getPadMediaType() == PAD_VIDEO) {
        videoPad = pad;
    } else if (pad->getPadMediaType() == PAD_AUDIO) {
        audioPad = pad;
    }
}

void DemuxElement::_start() {
    ALOGE("enter: %s", __FUNCTION__);

    while(isPlaying) {
        AVPacket* packet = av_packet_alloc();
        int ret = av_read_frame(mAvFormatContext, packet);
        if (ret == 0) {
            if (packet->stream_index == videoIndex) {
                if (videoPad != 0 && videoPad->hasObserver()) {
                    videoPad->notify(packet);
                } else {
                    av_packet_free(&packet);
                }
            } else if (packet->stream_index == audioIndex) {
                if (audioPad != 0 && audioPad->hasObserver()) {
                    audioPad->notify(packet);
                } else {
                    av_packet_free(&packet);
                }
            } else {
                av_packet_free(&packet);
            }
        } else if (ret == AVERROR_EOF) {
            //表示流已经读完了
            av_packet_free(&packet);
            break;
        } else {
            //读取数据包时发生错误
            ALOGE("read av stream fail");
            av_packet_free(&packet);
            mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, READ_PACKETS_FAIL);
            break;
        }
    }

    isPlaying = false;

    elementState = ELEMENT_STATE_STOP;
}




