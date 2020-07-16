//
// Created by llm on 20-5-11.
//
#include <pthread.h>
#include "DemuxElement.h"
#include "FFLog.h"
#include "macro.h"

void *demux_task_start(void *args) {
    ALOGE("enter: %s", __PRETTY_FUNCTION__);
    DemuxElement *element = static_cast<DemuxElement *>(args);
    element->_start();
    return 0;//一定一定一定要返回0！！！
}

DemuxElement::DemuxElement(const char *filepath) {
    elementType = DEMUX;

    this->filePath = new char[strlen(filepath) + 1];
    strcpy(this->filePath, filepath);
}

DemuxElement::~DemuxElement() {
    ALOGE("DemuxElement::~DemuxElement()");
    if (filePath != 0) {
        delete filePath;
        filePath = 0;
    }

    if (mNotify != 0) {
        mNotify = 0;
    }
}

int DemuxElement::open(PLAYER_PARAMETERS &avContext, notify_callback_f notifyFunc) {
    ALOGI("enter %s", __FUNCTION__);
    mNotify = notifyFunc;
    this->avContext = &avContext;

    this->avContext->formatContext = avformat_alloc_context();

    //打开一个输入流并解析Header
    //第3个参数是input format. 传0是自动检测
    //第4个参数是dictionary, 对于网络流可以设置超时,这里暂时不考虑网络流
    ALOGE("DemuxElement::open filePath : %s", filePath);
    int ret = avformat_open_input(&avContext.formatContext, filePath, 0, 0);
    if (ret != 0) {
        ALOGE("avformat open file fail!!!  %s",  av_err2str(ret));
        mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, OPEN_URL_FAIL);
        return OPEN_URL_FAIL;
    }

    ret = avformat_find_stream_info(avContext.formatContext, 0);
    if (ret < 0) {
        ALOGE("avformat_find_stream_info fail：%s", av_err2str(ret));
        mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, FIND_STREAM_FAIL);
        return FIND_STREAM_FAIL;
    }

    for (int i = 0; i < avContext.formatContext->nb_streams; i++) {
        int codecType = avContext.formatContext->streams[i]->codecpar->codec_type;
        switch (codecType) {
            case AVMEDIA_TYPE_VIDEO:
                avContext.videoIndex = i;
                break;
            case AVMEDIA_TYPE_AUDIO:
                avContext.audioIndex = i;
                break;
        }
    }


    if (avContext.videoIndex == -1 && avContext.audioIndex == -1) {
        ALOGE("stream don't contain video or audio");
        mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, FIND_STREAM_FAIL);
        return FIND_STREAM_FAIL;
    }

    elementState = ELEMENT_STATE_STOP;

    return STATUS_OK;
}

int DemuxElement::start() {
    ALOGE("enter: %s", __PRETTY_FUNCTION__);

    if (isPlaying == true && elementState == ELEMENT_STATE_PLAYING) {
        ALOGE("enter: %s player has already started", __FUNCTION__);
        return MEDIA_ALREAD_START;
    }
    isPlaying = true;

    if (elementState == ELEMENT_STATE_STOP) {
        int ret = av_seek_frame(avContext->formatContext, -1, 0, AVSEEK_FLAG_BACKWARD);
    }

    if (elementState != ELEMENT_STATE_PAUSE) {
        pthread_create(&pid_start, 0, demux_task_start, this);
    }
    elementState = ELEMENT_STATE_PLAYING;

    return STATUS_OK;
}

int DemuxElement::pause() {
    ALOGE("enter DemuxElement::pause() state %d", elementState);
    if (elementState == ELEMENT_STATE_PAUSE) {
        return STATUS_OK;
    } else if (elementState == ELEMENT_STATE_PLAYING) {
        elementState = ELEMENT_STATE_PAUSE;
        return STATUS_OK;
    } else {
        return INVALID_OPERATION;
    }
}

int DemuxElement::stop() {
    ALOGE("enter DemuxElement::stop");
    if (isPlaying) {
        isPlaying = false;

        //防止阻塞
        if (videoPad != 0 && videoPad->hasObserver()) {
            videoPad->getObserver()->setRecivieData(false);
        }

        if (audioPad != 0 && audioPad->hasObserver()) {
            audioPad->getObserver()->setRecivieData(false);
        }


        pthread_join(pid_start, 0);
    }

     //在start线程中会调用, 等线程结束后再删,防止线程冲突
    if (videoPad != 0) {
        if (videoPad->hasObserver()) {
            videoPad->setObserver(0);
        }
        delete videoPad;
        videoPad = 0;
    }

    if (audioPad != 0) {
        if (audioPad->hasObserver()) {
            audioPad->setObserver(0);
        }
        delete audioPad;
        audioPad = 0;
    }


    if (avContext->formatContext) {
        avformat_close_input(&avContext->formatContext);
        avformat_free_context(avContext->formatContext);
        avContext->formatContext = 0;
    }

    elementState = ELEMENT_STATE_STOP;

    return 0;
}

int DemuxElement::release() {
    ALOGE("enter DemuxElement::release");
    if (videoPad != 0) {
        delete videoPad;
        videoPad = 0;
    }

    if (audioPad != 0) {
        delete audioPad;
        audioPad = 0;
    }
    return 0;
}

int DemuxElement::reset() {
    return stop();
}


void DemuxElement::addPad(FFPad* pad) {
    if (pad->getPadMediaType() == PAD_VIDEO) {
        if (videoPad != 0) {
            delete videoPad;
        }
        videoPad = pad;
    } else if (pad->getPadMediaType() == PAD_AUDIO) {
        if (audioPad != 0) {
            delete audioPad;
        }
        audioPad = pad;
    }
}

void DemuxElement::_start() {
    ALOGE("enter: %s", __FUNCTION__);

    while(isPlaying) {

        if (elementState == ELEMENT_STATE_PAUSE) {
            av_usleep(10 * 1000);
            continue;
        }
        //由使用者进行管理和释放
        AVPacket* packet = av_packet_alloc();
        int ret = av_read_frame(avContext->formatContext, packet);
        if (ret == 0) {
            if (packet->stream_index == avContext->videoIndex) {
                if (videoPad != 0 && videoPad->hasObserver()) {
                    videoPad->notify(packet);
                } else {
                    av_packet_free(&packet);
                }
            } else if (packet->stream_index == avContext->audioIndex) {
                if (audioPad != 0 && audioPad->hasObserver()) {
                    audioPad->notify(packet);
                } else {
                    av_packet_free(&packet);
                }
            } else {
                //不是音频也不是视频流暂不处理
                av_packet_free(&packet);
            }
        } else if (ret == AVERROR_EOF) {
            //表示流已经读完了
            //要考虑读完了，是否播完了的情况
            av_packet_free(&packet);
            mNotify(MEDIA_INFO, MEDIA_INFO_EOF, 0);
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
    mNotify(MEDIA_INFO, MEDIA_INFO_ELEMENT_STATE_CHANGE, ELEMENT_STATE_STOP);
}

int DemuxElement::setSurface(ANativeWindow *window) {
    return INVALID_OPERATION;
}

bool DemuxElement::isDataBufferEmpty() {
    return true;
}




