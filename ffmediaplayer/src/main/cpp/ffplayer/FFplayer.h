//
// Created by llm on 20-4-22.
//

#ifndef FFMEDIAPLAYER_FFPLAYER_H
#define FFMEDIAPLAYER_FFPLAYER_H

#ifdef __cplusplus

#include <cstdint>
#include "../MediaPlayerInterface.h"

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

    //设置给各个element的回调函数
    static void notify(int msg, int ext1, int ext2);

private:
    char *filePath = 0;

    AVFormatContext *avformatContext = 0;

    //用于prepareAsync的异步线程handle
    pthread_t pid_prepare;
};

#endif // __cplusplus

#endif //FFMEDIAPLAYER_FFPLAYER_H
