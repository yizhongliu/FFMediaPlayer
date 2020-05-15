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
#include "FFLog.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

FFMediaPlayer::FFMediaPlayer() {
    ALOGE("FFMediaPlayer contruct");
    mCurrentState = MEDIA_PLAYER_IDLE;
}

FFMediaPlayer::~FFMediaPlayer() {
    ALOGE("FFMediaPlayer destruct");
}

void FFMediaPlayer::setListener(const std::shared_ptr<MediaPlayerListener>& listener) {
    mListener = listener;
}

void FFMediaPlayer::notify(int msg, int ext1, int ext2) {

}

int FFMediaPlayer::setDataSource(char *filePath) {
    std::shared_ptr<FactoryInterface> factoryInterface(new FFplayerFactory());
    std::shared_ptr<MediaPlayerInterface> p = factoryInterface -> createPlayer();

    if (p == NULL) {
        return CREATE_PLAYER_FAIL;
    }

    mPlayer = p;
    mPlayer->setNotifyCallback(notify);

    int ret = STATUS_OK;
    ret = mPlayer->setDataSource(filePath);
    if (ret == STATUS_OK) {
        mCurrentState = MEDIA_PLAYER_INITIALIZED;
    }

    return ret;
}

int FFMediaPlayer::prepareAsync() {
    if ( (mPlayer != 0) && (mCurrentState & (MEDIA_PLAYER_INITIALIZED | MEDIA_PLAYER_STOPPED)) ) {
        mCurrentState = MEDIA_PLAYER_PREPARING;
        return mPlayer->prepareAsync();
    }

    ALOGE("prepareAsync called in state %d, mPlayer(%p)", mCurrentState, mPlayer.get());
    return INVALID_OPERATION;
}

//TODO： remove Just for test
void *task_test(void *args) {
//    std::shared_ptr<MediaPlayerListener> listener = static_cast<std::shared_ptr<MediaPlayerListener>*>(args);
    auto listener = static_cast<std::shared_ptr<MediaPlayerListener>*>(args);
    (*listener)->notify(2,2,2);
    ALOGE("listener count %ld", (*listener).use_count());
    return 0;//一定一定一定要返回0！！！
}


//TODO： remove Just for test
void FFMediaPlayer::testCallback(bool newThread) {
    if (newThread) {
        pthread_t pid_test;
        pthread_create(&pid_test, 0, task_test, &mListener);
    } else {
        mListener->notify(1,1,1);
    }
}

//TODO： remove Just for test
void FFMediaPlayer::testCreatePlayer() {
    FactoryInterface* factoryInterface = new FFplayerFactory();
    mPlayer = factoryInterface ->createPlayer();
    ALOGE("mPlayeer count %ld", mPlayer.use_count());
    std::shared_ptr<MediaPlayerInterface> temp(mPlayer);
    ALOGE("mPlayeer count %ld", mPlayer.use_count());
    ALOGE("mPlayeer count2 %ld", temp.use_count());
    mPlayer.reset();
    ALOGE("mPlayeer count %ld", mPlayer.use_count());
    ALOGE("mPlayeer count2 %ld", temp.use_count());
}



