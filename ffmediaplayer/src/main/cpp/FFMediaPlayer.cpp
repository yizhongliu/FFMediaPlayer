//
// Created by yizho on 2020/3/23.
//

#include <pthread.h>
#include "FFMediaPlayer.h"
#include "macro.h"
#include "MediaPlayerInterface.h"
#include "FactoryInterface.h"

#include "ffplayer/FFplayerFactory.h"
#include "ffplayer/FFplayer.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

FFMediaPlayer::FFMediaPlayer() {
    //TODO:test code, remove
    ALOGE("avcodec version: %d", avcodec_version());
    mCurrentState = MEDIA_PLAYER_IDLE;
}

FFMediaPlayer::~FFMediaPlayer() {

}

void FFMediaPlayer::setListener(MediaPlayerListener *listener) {
    mListener = listener;
}

//TODO： remove Just for test
void *task_test(void *args) {
    MediaPlayerListener *listener = static_cast<MediaPlayerListener *>(args);
    listener->notify(2,2,2);
    return 0;//一定一定一定要返回0！！！
}


//TODO： remove Just for test
void FFMediaPlayer::testCallback(bool newThread) {
    if (newThread) {
        pthread_t pid_test;
        pthread_create(&pid_test, 0, task_test, mListener);
    } else {
        mListener->notify(1,1,1);
    }
}

//TODO： remove Just for test
void FFMediaPlayer::testCreatePlayer() {
    FactoryInterface* factoryInterface = new FFplayerFactory();
    mPlayer = factoryInterface ->createPlayer();
    ALOGE("mPlayeer count %d", mPlayer.use_count());
    std::shared_ptr<MediaPlayerInterface> temp(mPlayer);
    ALOGE("mPlayeer count %d", mPlayer.use_count());
    ALOGE("mPlayeer count2 %d", temp.use_count());
    mPlayer.reset();
    ALOGE("mPlayeer count %d", mPlayer.use_count());
    ALOGE("mPlayeer count2 %d", temp.use_count());
}



