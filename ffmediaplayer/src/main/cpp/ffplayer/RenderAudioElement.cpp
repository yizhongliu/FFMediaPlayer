//
// Created by llm on 20-6-22.
//

#include <FFLog.h>
#include <macro.h>
#include "RenderAudioElement.h"

extern "C" {
#include <libswresample/swresample.h>
};

void *audio_render_task_start(void *args) {
    ALOGE("enter: %s", __PRETTY_FUNCTION__);
    RenderAudioElement *element = static_cast<RenderAudioElement *>(args);
    element ->_start();
    return 0;//一定一定一定要返回0！！！
}

void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context) {
    ALOGE("bqPlayerCallback");
    RenderAudioElement *element = static_cast<RenderAudioElement *>(context);
    int pcm_size = element->getPCM();
    if (pcm_size > 0) {
        (*bq)->Enqueue(bq, element->outBuffers, pcm_size);
    }
}

RenderAudioElement::RenderAudioElement() {
    elementType = VIDEO_SINK;
}

RenderAudioElement::~RenderAudioElement() {
    ALOGE("RenderVideoElement::~RenderVideoElement() ");

    if (swrContext) {
        swr_free(&swrContext);
        swrContext = 0;
    }

    if (outBuffers) {
        delete outBuffers;
        outBuffers = 0;
    }
}

int RenderAudioElement::open(PLAYER_PARAMETERS &avContext, notify_callback_f notifyFunc) {

    this->avContext  = avContext;
    if (avContext.audioIndex == -1) {
        return INVALID_OPERATION;
    }

    /*
     * 设备可能不支持音频流的格式,需要进行重采样
     * */
    //FIXME:这里写死了, 应该想办法从设备获取
    outChannels = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
    outSampleSize = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    outSampleRate = 44100;
    // 2(通道数) * 2（16bit=2字节）*44100（采样率）
    outBuffersSize = outChannels * outSampleSize * outSampleRate;
    outBuffers = static_cast<uint8_t *>(malloc(outBuffersSize));
    memset(outBuffers, 0, outBuffersSize);

    ALOGE("RenderAudioElement::open channel : %d,  sampleFormat:%d , sampleRate:%d", avContext.audioCodecContext->channel_layout, avContext.audioCodecContext->sample_fmt, avContext.audioCodecContext->sample_rate);

    swrContext = swr_alloc_set_opts(0, AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16,
                                    outSampleRate,
                                    avContext.audioCodecContext->channel_layout,
                                    avContext.audioCodecContext->sample_fmt,
                                    avContext.audioCodecContext->sample_rate,
                                    0, 0);
    //初始化重采样上下文
    swr_init(swrContext);


    return 0;
}

int RenderAudioElement::start() {
    ALOGE("enter RenderAudioElement: %s", __PRETTY_FUNCTION__);

    if (audioPad == 0) {
        ALOGE("audio is 0");
        return INVALID_OPERATION;
    }

    if (isPlaying == true && elementState == ELEMENT_STATE_PLAYING) {
        ALOGE("enter: %s player has already started", __FUNCTION__);
        return MEDIA_ALREAD_START;
    }
    isPlaying = true;

    if (elementState != ELEMENT_STATE_PAUSE) {
        pthread_create(&pid_start, 0, audio_render_task_start, this);
    }
    elementState = ELEMENT_STATE_PLAYING;

    return STATUS_OK;
}

int RenderAudioElement::pause() {
    ALOGE("enter RenderAudioElement::pause() state %d", elementState);
    if (elementState == ELEMENT_STATE_PAUSE) {
        return STATUS_OK;
    } else if (elementState == ELEMENT_STATE_PLAYING) {
        elementState = ELEMENT_STATE_PAUSE;
        return STATUS_OK;
    } else {
        return INVALID_OPERATION;
    }
}

int RenderAudioElement::stop() {
    if (isPlaying) {
        isPlaying = false;
        pthread_join(pid_start, 0);
    }

    if (swrContext) {
        swr_free(&swrContext);
        swrContext = 0;
    }
    /**
    * 7、释放
    */
    //7.1 设置播放器状态为停止状态
    if (bqPlayerPlay) {
        (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
    //7.2 销毁播放器
    if (bqPlayerObject) {
        (*bqPlayerObject)->Destroy(bqPlayerObject);
        bqPlayerObject = 0;
        bqPlayerBufferQueue = 0;
    }
    //7.3 销毁混音器
    if (outputMixObject) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = 0;
    }
    //7.4 销毁引擎
    if (engineObject) {
        (*engineObject)->Destroy(engineObject);
        engineObject = 0;
        engineInterface = 0;
    }
    return 0;
}

int RenderAudioElement::release() {
    return stop();
}

int RenderAudioElement::reset() {
    return stop();
}

bool RenderAudioElement::isDataBufferEmpty() {
    return audioPad->isDataBufferEmpty();
}

void RenderAudioElement::addPad(FFPad *pad) {
    if (pad->getPadMediaType() == PAD_AUDIO && pad->getPadType() == PAD_SINK) {
        if (audioPad != 0) {
            delete audioPad;
        }
        audioPad = pad;
    }
}

void RenderAudioElement::_start() {
    /**
        * 1、创建引擎并获取引擎接口
        */
    SLresult result;
    // 1.1 创建引擎对象：SLObjectItf engineObject
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    // 1.2 初始化引擎
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    // 1.3 获取引擎接口 SLEngineItf engineInterface
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineInterface);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    /**
     * 2、设置混音器
     */
    // 2.1 创建混音器：SLObjectItf outputMixObject
    result = (*engineInterface)->CreateOutputMix(engineInterface, &outputMixObject, 0,
                                                 0, 0);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    // 2.2 初始化混音器
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    /**
     * 3、创建播放器
     */
    //3.1 配置输入声音信息
    //创建buffer缓冲类型的队列 2个队列
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                       2};
    //pcm数据格式
    //SL_DATAFORMAT_PCM：数据格式为pcm格式
    //2：双声道
    //SL_SAMPLINGRATE_44_1：采样率为44100
    //SL_PCMSAMPLEFORMAT_FIXED_16：采样格式为16bit
    //SL_PCMSAMPLEFORMAT_FIXED_16：数据大小为16bit
    //SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT：左右声道（双声道）
    //SL_BYTEORDER_LITTLEENDIAN：小端模式
    SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 2, SL_SAMPLINGRATE_44_1,
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_PCMSAMPLEFORMAT_FIXED_16,
                                   SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
                                   SL_BYTEORDER_LITTLEENDIAN};

    //数据源 将上述配置信息放到这个数据源中
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};

    //3.2 配置音轨（输出）
    //设置混音器
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};
    //需要的接口 操作队列的接口
    const SLInterfaceID ids[1] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    //3.3 创建播放器
    result = (*engineInterface)->CreateAudioPlayer(engineInterface, &bqPlayerObject, &audioSrc,
                                                   &audioSnk, 1, ids, req);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //3.4 初始化播放器：SLObjectItf bqPlayerObject
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    //3.5 获取播放器接口：SLPlayItf bqPlayerPlay
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    if (SL_RESULT_SUCCESS != result) {
        return;
    }
    /**
     * 4、设置播放回调函数
     */
    //4.1 获取播放器队列接口：SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue
    (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_BUFFERQUEUE, &bqPlayerBufferQueue);

    //4.2 设置回调 void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
    (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, bqPlayerCallback, this);

    /**
     * 5、设置播放器状态为播放状态
     */
    (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);

    /**
     * 6、手动激活回调函数
     */
    bqPlayerCallback(bqPlayerBufferQueue, this);
}

int RenderAudioElement::getPCM() {
    int pcmDataSize = 0;
    AVFrame *frame = 0;

    while (isPlaying) {

   //     ALOGE("RenderAudioElement::getPCM");

        if (elementState == ELEMENT_STATE_PAUSE) {
            av_usleep(10 * 1000);
            continue;
        }

  //      ALOGE("RenderAudioElement getData");
        AVFrame* frame = (AVFrame *) audioPad->getData();
        if (frame == 0) {
   //         ALOGE("RenderAudioElement::getPCM() getData is 0");
            av_usleep(10 * 1000);
            continue;
        }


        //pcm数据在 frame中
        //这里获得的解码后pcm格式的音频原始数据，有可能与创建的播放器中设置的pcm格式不一样, 需要进行重采样 resample

        //假设输入10个数据，有可能这次转换只转换了8个，还剩2个数据（delay）
        //断点：1024 * 48000

        //swr_get_delay: 下一个输入数据与下下个输入数据之间的时间间隔
        int64_t delay = swr_get_delay(swrContext, frame->sample_rate);

        //a * b / c
        //AV_ROUND_UP：向上取整
        int64_t out_max_samples = av_rescale_rnd(frame->nb_samples + delay, frame->sample_rate,
                                                 outSampleRate,
                                                 AV_ROUND_UP);

        //上下文
        //输出缓冲区
        //输出缓冲区能容纳的最大数据量
        //输入数据
        //输入数据量
        int out_samples = swr_convert(swrContext, &outBuffers, out_max_samples,
                                      (const uint8_t **) (frame->data), frame->nb_samples);

        // 获取swr_convert转换后 out_samples个 *2 （16位）*2（双声道）
        pcmDataSize = out_samples * outSampleSize * outChannels;
//        frame->best_effort_timestamp*时间单位

        //获取音频时间 audio_time需要被VideoChannel获取
        //audio_time = frame->best_effort_timestamp * av_q2d(time_base);

        break;

    }//end while

    if (frame) {
        av_frame_free(&frame);
        frame = 0;
    }

    return pcmDataSize;
}

int RenderAudioElement::setSurface(ANativeWindow *window) {
    return INVALID_OPERATION;
}



