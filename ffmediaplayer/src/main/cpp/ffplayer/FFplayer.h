//
// Created by llm on 20-4-22.
//

#ifndef FFMEDIAPLAYER_FFPLAYER_H
#define FFMEDIAPLAYER_FFPLAYER_H

#ifdef __cplusplus

#include <cstdint>
#include "../MediaPlayerInterface.h"

class FFplayer : public MediaPlayerInterface {
public:
    FFplayer();
    ~FFplayer();

    virtual int    setDataSource(int fd, int64_t offset, int64_t length);
    virtual int    prepareAsync();
    virtual int    start();
    virtual int    stop();
    virtual int    pause();
    virtual int    reset();
    virtual int    release();
};

#endif // __cplusplus

#endif //FFMEDIAPLAYER_FFPLAYER_H
