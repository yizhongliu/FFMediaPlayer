//
// Created by llm on 20-4-22.
//

#ifndef FFMEDIAPLAYER_FFPLAYER_H
#define FFMEDIAPLAYER_FFPLAYER_H

#ifdef __cplusplus

#include <cstdint>
#include "MediaPlayerInterface.h"
#include "FFPad.h"
#include "FFElement.h"
#include "DecodeVideoElement.h"
#include "RenderVideoElement.h"

extern "C" {
#include <libavformat/avformat.h>
};


class FFplayer : public MediaPlayerInterface {
public:
    FFplayer();
    ~FFplayer();

    virtual int    setDataSource(char * filePath);
    virtual int    prepare();
    virtual int    prepareAsync();
    virtual int    start();
    virtual int    stop();
    virtual int    pause();
    virtual int    reset();
    virtual int    release();
    virtual int    setSurface(ANativeWindow* window);

    //设置给各个element的回调函数
    static void notify(int msg, int ext1, int ext2);

    void linkPad(FFElement* element, FFPad* pad);
    void connectPad(FFPad* src, FFPad* observer);

    void initPlayerParameters();
    void releasePlayerParameters();

    void _start();

    FFElement* getVidoRender();

private:
    char *filePath = 0;

    //各个element中共享的参数
    PLAYER_PARAMETERS avContext;

    //用于prepareAsync的异步线程handle
    pthread_t pid_prepare;

    //用于播放是否正常播放完的判断
    pthread_t pid_start;

    ANativeWindow *window = 0;

    static FFplayer * pThis;
    list<FFElement*> elements;

    bool isPlaying = false;
    bool bReachEOF = false;
    element_state elementState;

};

#endif // __cplusplus

#endif //FFMEDIAPLAYER_FFPLAYER_H
