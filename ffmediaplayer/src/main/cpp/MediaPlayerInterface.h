//
// Created by llm on 20-4-22.
//

#ifndef FFMEDIAPLAYER_MEDIAPLAYERINTERFACE_H
#define FFMEDIAPLAYER_MEDIAPLAYERINTERFACE_H

#include <mutex>

#ifdef __cplusplus

enum player_type {
    FFMPEG_PLAYER = 0,
};

// callback mechanism for passing messages to MediaPlayer object
typedef void (*notify_callback_f)(int msg, int ext1, int ext2);


class MediaPlayerInterface {
public:
    virtual  ~MediaPlayerInterface() {}

    virtual int    setDataSource(int fd, int64_t offset, int64_t length) = 0;
    virtual int    prepareAsync() = 0;
    virtual int    start() = 0;
    virtual int    stop() = 0;
    virtual int    pause() = 0;
    virtual int    reset() = 0;
    virtual int    release() = 0;

    //设置回调函数
    void  setNotifyCallback(notify_callback_f notifyFunc) {
        std::lock_guard<std::mutex> lock(mNotifyLock);
        mNotify = notifyFunc;
    }

    //发送回调消息
    void sendEvent(int msg, int ex1 = 0, int ext2 = 0) {
        std::lock_guard<std::mutex> lock(mNotifyLock);
        if (mNotify) {
            mNotify(msg, ex1, ext2);
        }
    }

private:
    notify_callback_f mNotify = 0;
    std::mutex mNotifyLock;
};

#endif // __cplusplus


#endif //FFMEDIAPLAYER_MEDIAPLAYERINTERFACE_H
