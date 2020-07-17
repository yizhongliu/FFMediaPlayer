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

/*
 * 根据pad 是用于接收数据还是输出数据分类
 * */
enum pad_type {
    PAD_SOURCE = 0,
    PAD_SINK
};

/*
 * 根据pad 处理的媒体数据类型来分类
 * */
enum pad_media_type {
    PAD_VIDEO = 0,
    PAD_AUDIO
};

/*
 * ffmpeg 中编码数据和raw数据来分类
 * */
enum pad_data_type {
    PAD_DATA_PACKET = 0,
    PAD_DATA_FRAME
};

typedef void (*chain_func)(void* data);


class FFPad : public IObserver{
public:
    FFPad();
    FFPad(pad_type padType, pad_media_type mediaType);
    virtual ~FFPad();

    pad_type getPadType();

    pad_media_type getPadMediaType();

    /*
     * sink pad 接收数据的函数，由source pad 通过observer 调用
     * 当buffer满了后, 函数会阻塞
     * */
    virtual void addData(void* data) = 0;

    /*
     * element 通过该方法获取sink pad的数据
     * */
    virtual void* getData() = 0;

    /*
     *
     * */
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

    //接收数据时,sink pad的缓冲buffer满了后会阻塞. 当停止播放器时,需要防止addData继续阻塞
    //当为false时，则sink pad 接收数据的回调函数会跳出循环
    bool receiveData = true;
};

#endif //FFMEDIAPLAYER_PAD_H
