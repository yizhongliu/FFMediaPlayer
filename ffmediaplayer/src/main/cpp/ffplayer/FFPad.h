//
// Created by llm on 20-5-15.
//

#ifndef FFMEDIAPLAYER_PAD_H
#define FFMEDIAPLAYER_PAD_H


#include "IObserver.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
}

enum pad_type {
    PAD_SOURCE = 0,
    PAD_SINK
};

enum pad_media_type {
    PAD_VIDEO = 0,
    PAD_AUDIO
};

typedef void (*chain_func)(void* data);


class FFPad : public IObserver{
public:
    FFPad();
    FFPad(pad_type padType, pad_media_type mediaType);
    virtual ~FFPad();

    pad_type getPadType();

    pad_media_type getPadMediaType();


    /*当buffer满了后, 函数会阻塞*/
    virtual void addData(void* data) = 0;

    virtual void* getData() = 0;

    virtual bool isDataBufferEmpty() = 0;

    bool isReceiveData();
    void setRecivieData(bool flag);

    /**
 * 释放 AVPacket
 * @param packet
 */
    static void releaseAVPacket(AVPacket **packet) {
        if (packet) {
            av_packet_free(packet);
            *packet = 0;
        }
    }

    /**
 * 释放 AVFrame
 * @param frame
 */
    static void releaseAVFrame(AVFrame **frame) {
        //     LOGE("releaveAvframe");
        if (frame) {

            av_frame_free(frame);
            *frame = 0;
        }
    }

protected:
    pad_type padType;
    pad_media_type mediaType;

    //接收数据时,pad的缓冲buffer满了后会阻塞. 当停止播放器时,需要防止addData继续阻塞
    bool receiveData = true;
};

#endif //FFMEDIAPLAYER_PAD_H
