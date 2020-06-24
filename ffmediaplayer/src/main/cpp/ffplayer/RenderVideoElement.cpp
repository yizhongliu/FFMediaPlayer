//
// Created by llm on 20-6-10.
//

#include "RenderVideoElement.h"
#include <android/native_window_jni.h>
#include <pthread.h>
#include <macro.h>
#include <FFLog.h>
#include <cstring>

void *video_render_task_start(void *args) {
    ALOGE("enter: %s", __PRETTY_FUNCTION__);
    RenderVideoElement *element = static_cast<RenderVideoElement *>(args);
    element ->_start();
    return 0;//一定一定一定要返回0！！！
}


RenderVideoElement::RenderVideoElement() {
    elementType = VIDEO_SINK;
}

RenderVideoElement::~RenderVideoElement() {
    ALOGE("RenderVideoElement::~RenderVideoElement() ");
}

int RenderVideoElement::open(PLAYER_PARAMETERS &avContext, notify_callback_f notifyFunc) {
    //要注意对原始数据进行格式转换：yuv > rgba
    // yuv: 400x800 > rgba: 400x800

    AVCodecContext *codecContext = avContext.videoCodecContext;
    width = codecContext->width;
    height = codecContext->height;
    sws_ctx = sws_getContext(codecContext->width, codecContext->height,
                                         codecContext->pix_fmt,
                                         codecContext->width, codecContext->height, AV_PIX_FMT_RGBA,
                                         SWS_BILINEAR, NULL, NULL, NULL);

    ALOGE("codecContext->width %d, codecContext->height %d", codecContext->width, codecContext->height);
    //给 dst_data dst_linesize 申请内存
    av_image_alloc(dst_data, dst_linesize,
                   codecContext->width, codecContext->height, AV_PIX_FMT_RGBA, 1);
    return 0;
}

int RenderVideoElement::start() {
    ALOGE("enter: %s", __PRETTY_FUNCTION__);

    if (videoPad == 0) {
        ALOGE("video is 0");
        return INVALID_OPERATION;
    }

    if (isPlaying == true && elementState == ELEMENT_STATE_PLAYING) {
        ALOGE("enter: %s player has already started", __FUNCTION__);
        return MEDIA_ALREAD_START;
    }
    isPlaying = true;

    if (elementState != ELEMENT_STATE_PAUSE) {
        pthread_create(&pid_start, 0, video_render_task_start, this);
    }
    elementState = ELEMENT_STATE_PLAYING;

    return STATUS_OK;
}

int RenderVideoElement::pause() {
    ALOGE("enter RenderVideoElement::pause() state %d", elementState);
    if (elementState == ELEMENT_STATE_PAUSE) {
        return STATUS_OK;
    } else if (elementState == ELEMENT_STATE_PLAYING) {
        elementState = ELEMENT_STATE_PAUSE;
        return STATUS_OK;
    } else {
        return INVALID_OPERATION;
    }
}

int RenderVideoElement::stop() {
    if (isPlaying) {
        isPlaying = false;
        pthread_join(pid_start, 0);
    }

    if (videoPad != 0) {
        delete videoPad;
        videoPad = 0;
    }

    av_freep(&dst_data[0]);
    if (sws_ctx) {
        sws_freeContext(sws_ctx);
        sws_ctx = 0;
    }

    if (window) {
 //       ANativeWindow_release(window);
        window = 0;
    }

    return 0;
}

int RenderVideoElement::release() {
    if (videoPad != 0) {
        delete videoPad;
        videoPad = 0;
    }

    av_freep(&dst_data[0]);
    sws_freeContext(sws_ctx);

    return 0;
}

int RenderVideoElement::reset() {
    stop();
    return 0;
}

void RenderVideoElement::addPad(FFPad *pad) {
    if (pad->getPadMediaType() == PAD_VIDEO) {
        if (videoPad != 0) {
            delete videoPad;
        }
        videoPad = pad;
    }
}


void RenderVideoElement::_start() {
    while (isPlaying) {

        if (elementState == ELEMENT_STATE_PAUSE) {
            av_usleep(10 * 1000);
            continue;
        }

        AVFrame* frame = (AVFrame *) videoPad->getData();
        if (frame == 0) {
    //        ALOGE("RenderVideoElement::_start()  frame is 0");
            av_usleep(10 * 1000);
            continue;
        }

        //取到了yuv原始数据，下面要进行格式转换
        sws_scale(sws_ctx, frame->data,
                  frame->linesize, 0, height, dst_data, dst_linesize);


        renderFrame(dst_data[0], dst_linesize[0], width, height);

        av_frame_free(&frame);
        frame = 0;
    }
}

int RenderVideoElement::setSurface(ANativeWindow * window) {

    pthread_mutex_lock(&mutex);

    ALOGE("RenderVideoElement::setSurface");
    if (this->window) {
        ANativeWindow_release(this->window);
    }
    this->window = window;

    pthread_mutex_unlock(&mutex);
    return 0;
}

//1，data;2，linesize；3，width; 4， height
void RenderVideoElement::renderFrame(uint8_t *src_data, int src_lineSize, int width, int height) {
    pthread_mutex_lock(&mutex);
    if (!window) {
        pthread_mutex_unlock(&mutex);
        return;
    }
//    ALOGE("RenderVideoElement width:%d, height:%d", width,height);
    ANativeWindow_setBuffersGeometry(window, width,
                                     height,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer window_buffer;
    if (ANativeWindow_lock(window, &window_buffer, 0)) {
        ANativeWindow_release(window);
        window = 0;
        pthread_mutex_unlock(&mutex);
        return;
    }
    //把buffer中的数据进行赋值（修改）
    uint8_t *dst_data = static_cast<uint8_t *>(window_buffer.bits);
    int dst_lineSize = window_buffer.stride * 4;//ARGB
    //逐行拷贝
  //  ALOGE("buffer.height : %d dst+lineSize : %d  src_lineSize : %d", window_buffer.height, dst_lineSize, src_lineSize);
    for (int i = 0; i < window_buffer.height; ++i) {
        memcpy(dst_data + i * dst_lineSize, src_data + i * src_lineSize, dst_lineSize);
    }
    ANativeWindow_unlockAndPost(window);
    pthread_mutex_unlock(&mutex);
}

bool RenderVideoElement::isDataBufferEmpty() {
    return videoPad->isDataBufferEmpty();
}




