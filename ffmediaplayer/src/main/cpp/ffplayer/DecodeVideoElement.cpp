//
// Created by llm on 20-5-16.
//

#include <pthread.h>

#include "DecodeVideoElement.h"
#include "FFLog.h"
#include "macro.h"

extern "C" {
#include <libavutil/time.h>
}

void *video_decode_task_start(void *args) {
    ALOGE("enter: %s", __PRETTY_FUNCTION__);
    DecodeVideoElement *element = static_cast<DecodeVideoElement *>(args);
    element ->_start();
    return 0;//一定一定一定要返回0！！！
}

DecodeVideoElement::DecodeVideoElement() {
    ALOGE("DecodeVideoElement::DecodeVideoElement()");
    elementType = VIDEO_DECODER;
}

DecodeVideoElement::~DecodeVideoElement() {
    ALOGE("DecodeVideoElement::~DecodeVideoElement() ");

    if (mNotify != 0) {
        mNotify = 0;
    }
}

int DecodeVideoElement::open(PLAYER_PARAMETERS &avContext, notify_callback_f notifyFunc) {
    mNotify = notifyFunc;

    if (avContext.videoIndex == -1) {
        ALOGE("不包含视频流");
        return INVALID_OPERATION;
    }

    //获取媒体流（
    AVStream *stream = avContext.formatContext->streams[avContext.videoIndex];
    //获取编解码这段流的参数
    AVCodecParameters *codecParameters = stream->codecpar;
    ALOGE("解码 id ：%d, type: %d", codecParameters->codec_id, codecParameters->codec_type);

    //通过参数中的id（编解码的方式），来查找当前流的解码器
    AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);
    if (!codec) {
        ALOGE("can't find video decoder");
        mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, FIND_DECODER_FAIL);
        return FIND_DECODER_FAIL;
    }

    //创建解码器上下文
    codecContext = avcodec_alloc_context3(codec);

    if (!codecContext) {
        ALOGE("创建解码器上下文失败");
        mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, ALLOC_CODEC_CONTEXT_FAIL);
        return ALLOC_CODEC_CONTEXT_FAIL;
    }
    //7 设置解码器上下文的参数
    int ret = avcodec_parameters_to_context(codecContext, codecParameters);
    if (ret < 0) {
        ALOGE("设置解码器上下文的参数失败：%s", av_err2str(ret));
        mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, CODEC_CONTEXT_PARAMETERS_FAIL);
        return CODEC_CONTEXT_PARAMETERS_FAIL;
    }
    //8 打开解码器
    ret = avcodec_open2(codecContext, codec, 0);
    if (ret) {
        ALOGE("打开解码器失败：%s", av_err2str(ret));
        mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, OPEN_DECODER_FAIL);
        return OPEN_DECODER_FAIL;
    }

    avContext.videoCodecContext = codecContext;

    elementState = ELEMENT_STATE_OPEN;

    return STATUS_OK;
}

int DecodeVideoElement::start() {
    ALOGE("enter: DecodeVideoElement::start() state %d", elementState);

    if (sinkPad == 0 || sourcePad == 0) {
        ALOGE("sinkPad is 0");
        return INVALID_OPERATION;
    }

    if (isPlaying == true && elementState == ELEMENT_STATE_PLAYING) {
        ALOGE("enter: %s player has already started", __FUNCTION__);
        return MEDIA_ALREAD_START;
    }
    isPlaying = true;

    if (elementState != ELEMENT_STATE_PAUSE) {
        pthread_create(&pid_start, 0, video_decode_task_start, this);
    }
    elementState = ELEMENT_STATE_PLAYING;

    return STATUS_OK;
}

int DecodeVideoElement::pause() {
    ALOGE("enter DecodeVideoElement::pause() state %d", elementState);
    if (elementState == ELEMENT_STATE_PAUSE) {
        return STATUS_OK;
    } else if (elementState == ELEMENT_STATE_PLAYING) {
        elementState = ELEMENT_STATE_PAUSE;
        return STATUS_OK;
    } else {
        return INVALID_OPERATION;
    }
}

int DecodeVideoElement::stop() {
    ALOGE("enter DecodeVideoElement::stop()");
    if (isPlaying) {
        isPlaying = false;

        //防止阻塞
        if (sourcePad != 0 && sourcePad->hasObserver()) {
            sourcePad->getObserver()->setRecivieData(false);
        }

        pthread_join(pid_start, 0);
    }

    //在start线程中会调用, 等线程结束后再删,防止线程冲突
    if (sourcePad != 0) {
        if (sourcePad->hasObserver()) {
            sourcePad->setObserver(0);
        }
        delete sourcePad;
        sourcePad = 0;
    }

    if (sinkPad != 0) {
        delete sinkPad;
        sinkPad = 0;
    }

//    if (codecContext) {
//        avcodec_close(codecContext);
//        avcodec_free_context(&codecContext);
//        codecContext = 0;
//    }

    return 0;
}

int DecodeVideoElement::release() {
    if (sinkPad != 0) {
        delete sinkPad;
        sinkPad = 0;
    }

    if (sourcePad != 0) {
        delete sourcePad;
        sourcePad = 0;
    }
    return 0;
}

int DecodeVideoElement::reset() {
    return stop();
}

void DecodeVideoElement::connectPads(FFPad *sourcePad, FFPad *sinkPad) {
    sourcePad->setObserver(sinkPad);
}

void DecodeVideoElement::addPad(FFPad* pad) {
 //   pads.push_back(pad);

    if (pad->getPadType() == PAD_SINK) {
        if (sinkPad != 0) {
            delete sinkPad;
        }
        sinkPad = pad;
    } else if (pad->getPadType() == PAD_SOURCE) {
        if (sourcePad != 0) {
            delete sourcePad;
        }
        sourcePad = pad;
    }
}

void DecodeVideoElement::_start() {
    ALOGE("enter: %s", __FUNCTION__);

    while (isPlaying) {

        if (elementState == ELEMENT_STATE_PAUSE) {
            av_usleep(10 * 1000);
            continue;
        }

        AVPacket* packet = (AVPacket *)sinkPad -> getData();
        if (packet == 0) {
     //       ALOGE("DecodeVideoElement::_start packet is 0");
            av_usleep(10 * 1000);
            continue;
        }

        //拿到了视频数据包（编码压缩了的），需要把数据包给解码器进行解码
        int ret = avcodec_send_packet(codecContext, packet);
        if (ret != 0) {
            //往解码器发送数据包失败，跳出循环
            ALOGE("avcodec_send_packet fail %d", ret );
            av_packet_free(&packet);//释放packet，后面不需要了
            packet = 0;

            mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, PASER_PACKETS_FAIL);
            break;

        }

        av_packet_free(&packet);//释放packet，后面不需要了
        packet = 0;

        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(codecContext, frame);
        if (ret == AVERROR(EAGAIN)) {
            //  AVERROR（EAGAIN）返回值意味着需要新的输入数据才能返回新的输出。
            // 在解码或编码开始时，编解码器可能会接收多个输入帧/数据包而不返回帧，直到其内部缓冲区被填充为止
            av_frame_free(&frame);
            frame = 0;
            continue;
        } else if (ret != 0) {
            ALOGE("avcodec_receive_frame fail %d", ret);
            av_frame_free(&frame);
            frame = 0;

            mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, PASER_PACKETS_FAIL);
            break;
        }

        if (sourcePad != 0 && sourcePad->hasObserver()) {
            sourcePad->notify(frame);
        } else {
            av_frame_free(&frame);
            frame = 0;
        }
    }
}

int DecodeVideoElement::setSurface(ANativeWindow *window) {
    return INVALID_OPERATION;
}

bool DecodeVideoElement::isDataBufferEmpty() {
    return sinkPad->isDataBufferEmpty();
}
