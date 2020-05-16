//
// Created by llm on 20-5-16.
//

#include "DecodeVideoElement.h"
#include "FFLog.h"
#include "macro.h"

int DecodeVideoElement::open(AVFormatContext *avFormatContext, notify_callback_f notifyFunc) {
    mNotify = notifyFunc;

    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        int codecType = avFormatContext->streams[i]->codecpar->codec_type;
        if (codecType == AVMEDIA_TYPE_VIDEO) {
            //获取媒体流（
            AVStream *stream = avFormatContext->streams[i];
            //4获取编解码这段流的参数
            AVCodecParameters *codecParameters = stream->codecpar;
            ALOGE("解码 id ：%d, type: %d", codecParameters->codec_id, codecParameters->codec_type);
            AVCodec *codec = avcodec_find_decoder(codecParameters->codec_id);
            if (!codec) {
                ALOGE("can't find video decoder");
                mNotify(MEDIA_ERROR, MEDIA_ERROR_UNKNOWN, FIND_DECODER_FAIL);
                return FIND_DECODER_FAIL;
            }
        }
    }
    return 0;
}

int DecodeVideoElement::start() {
    return 0;
}

int DecodeVideoElement::pause() {
    return 0;
}

int DecodeVideoElement::stop() {
    return 0;
}

int DecodeVideoElement::release() {
    return 0;
}

int DecodeVideoElement::reset() {
    return 0;
}
