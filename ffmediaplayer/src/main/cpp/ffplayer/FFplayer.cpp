//
// Created by llm on 20-4-22.
//

#include <android/native_window_jni.h>
#include "FFplayer.h"
#include "macro.h"
#include "FFLog.h"
#include "DemuxElement.h"
#include "DemuxVideoPad.h"
#include "DecodeVideoElement.h"
#include "DecodeVideoSinkPad.h"
#include "DecodeVideoSourcePad.h"
#include "RenderVideoPad.h"

/**
 * 准备线程pid_prepare真正执行的函数
 * @param args
 * @return
 */
void *task_prepare(void *args) {
    FFplayer *ffmpeg = static_cast<FFplayer *>(args);
    ffmpeg->prepare();
    return 0; //一定要return
}

void *player_task_start(void *args) {
    FFplayer *ffmpeg = static_cast<FFplayer *>(args);
    ffmpeg ->_start();
    return 0;
}

FFplayer * FFplayer::pThis = 0;
FFplayer::FFplayer() {
    ALOGE("FFplayer contruct");
    pThis = this;
    initPlayerParameters();
}

FFplayer::~FFplayer() {
    ALOGE("FFplayer decontruct");

    if (filePath != 0) {
        delete filePath;
        filePath = 0;
    }
}

void FFplayer::notify(int msg, int ext1, int ext2) {
    switch (msg) {
        case MEDIA_INFO:
            switch (ext1) {
                case MEDIA_INFO_EOF:
                    pThis -> bReachEOF = true;
                    break;
                case MEDIA_INFO_ELEMENT_STATE_CHANGE:
                    break;
                default:
                    pThis -> sendEvent(msg, ext1, ext2);
                    break;
            }

            break;
        default:
            pThis -> sendEvent(msg, ext1, ext2);
            break;
    }
}

int FFplayer::setDataSource(char * filePath) {
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);

    return 0;
}

int FFplayer::prepare() {
    ALOGI("Enter %s", __FUNCTION__);

    int ret = 0;

    //创建demux模块
    ALOGE("filepath: %s", filePath);
    FFElement* demuxElement = new DemuxElement(filePath);
    ret = demuxElement->open(avContext, notify);
    if (ret != STATUS_OK) {
        return ret;
    }

    FFPad* demuxVideoPad = new DemuxVideoPad();
    linkPad(demuxElement, demuxVideoPad);

    //创建decode模块
    FFElement* decodeVideoElement = new DecodeVideoElement();
    ret = decodeVideoElement->open(avContext, notify);
    if (ret != STATUS_OK) {
        return ret;
    }

    FFPad* videoDecodeSinkPad = new DecodeVideoSinkPad();
    linkPad(decodeVideoElement, videoDecodeSinkPad);
    FFPad* videoDecodeSourcePad = new DecodeVideoSourcePad();
    linkPad(decodeVideoElement, videoDecodeSourcePad);

    //将demux的 输出的pad 链接到 decode输入pad
    connectPad(demuxVideoPad, videoDecodeSinkPad);

    //创建 render模块
    FFElement* renderVideoElement = new RenderVideoElement();
    ret = renderVideoElement->open(avContext, notify);
    if (ret != STATUS_OK) {
        return ret;
    }
    renderVideoElement->setSurface(window);

    FFPad* videoRenderPad = new RenderVideoPad();
    linkPad(renderVideoElement, videoRenderPad);

    //将decode 和 render建立连接
    connectPad(videoDecodeSourcePad, videoRenderPad);

    elements.push_back(demuxElement);
    elements.push_back(decodeVideoElement);
    elements.push_back(renderVideoElement);

    elementState = ELEMENT_STATE_OPEN;

    sendEvent(MEDIA_PREPARED);
    return 0;

}

int FFplayer::prepareAsync() {
    ALOGI("FFplayer::prepareAsync");
    pthread_create(&pid_prepare, 0, task_prepare, this);
    return 0;
}

int FFplayer::start() {
    ALOGE("FFplayer::start");

    if (isPlaying == true && elementState == ELEMENT_STATE_PLAYING) {
        ALOGE("enter: %s player has already started", __FUNCTION__);
        return MEDIA_ALREAD_START;
    }

    isPlaying = true;
    bReachEOF = false;

    if (elementState != ELEMENT_STATE_PAUSE) {
        pthread_create(&pid_start, 0, player_task_start, this);
    }

    list<FFElement*>::iterator it;

    for(it = elements.begin(); it != elements.end(); it++) {
        (*it) ->start();
    }

    elementState = ELEMENT_STATE_PLAYING;
    return 0;
}

int FFplayer::stop() {
    ALOGE("FFplayer::stop");
    isPlaying = false;
    list<FFElement*>::iterator it;

    for(it = elements.begin(); it != elements.end(); it++) {
        (*it) ->stop();
    }

    for(it = elements.begin(); it != elements.end(); it++) {
        delete (*it);
        it = elements.erase(it);
    }

    elementState = ELEMENT_STATE_STOP;

    return 0;
}

int FFplayer::pause() {
    ALOGE("FFplayer::pause()");
    list<FFElement*>::iterator it;

    int ret = STATUS_OK;
    for(it = elements.begin(); it != elements.end(); it++) {
        ret = (*it) ->pause();
        if (ret != STATUS_OK) {
            ALOGE("pause fail status :%d", ret);
            break;
        }
    }

    elementState = ELEMENT_STATE_PAUSE;

    return STATUS_OK;
}

int FFplayer::reset() {
    ALOGE("FFplayer::reset");
    return stop();
}

int FFplayer::release() {
    return stop();
}

void FFplayer::linkPad(FFElement* element, FFPad* pad) {
    element->addPad(pad);
}

void FFplayer::connectPad(FFPad *src, FFPad *observer) {
    src->setObserver(observer);
}

int FFplayer::setSurface(ANativeWindow* window) {

    this->window = window;
    FFElement* element = getVidoRender();
    if (element) {
        return element->setSurface(window);
    }
    return STATUS_OK;
}

void FFplayer::initPlayerParameters() {
    avContext.formatContext = 0;
    avContext.audioCodecContext = 0;
    avContext.videoCodecContext = 0;

    avContext.audioIndex = -1;
    avContext.videoIndex = -1;
}

FFElement *FFplayer::getVidoRender() {
    list<FFElement*>::iterator it;

    for(it = elements.begin(); it != elements.end(); it++) {
        if ((*it) ->getElementType() == VIDEO_SINK) {
            return (*it);
        }
    }

    return 0;
}

void FFplayer::_start() {

    list<FFElement*>::iterator it;
    bool bComplete = false;

    while (isPlaying) {
        if (!bReachEOF) {
            av_usleep(100 * 1000);
            continue;
        }

        bComplete = true;
        for(it = elements.begin(); it != elements.end(); it++) {
            if (!(*it) ->isDataBufferEmpty()) {
                bComplete = false;
                break;
            }
        }

        if (!bComplete) {
            av_usleep(100 * 1000);
            continue;
        } else {
            break;
        }
    }

    isPlaying = false;

    for(it = elements.begin(); it != elements.end(); it++) {
        (*it) -> setReachEOF();
    }

    elementState = ELEMENT_STATE_STOP;

    sendEvent(MEDIA_PLAYBACK_COMPLETE, 0, 0);
}



