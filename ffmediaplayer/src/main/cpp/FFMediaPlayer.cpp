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

FFMediaPlayer * FFMediaPlayer::pThis = 0;
FFMediaPlayer::FFMediaPlayer() {
    ALOGE("FFMediaPlayer contruct");
    pThis = this;
    mCurrentState = MEDIA_PLAYER_IDLE;
}

FFMediaPlayer::~FFMediaPlayer() {
    ALOGE("FFMediaPlayer destruct");
}

void FFMediaPlayer::setListener(const std::shared_ptr<MediaPlayerListener>& listener) {
    mListener = listener;
}

void FFMediaPlayer::notify(int msg, int ext1, int ext2) {
    ALOGD("FFMediaPlayer::notify(%d, %d, %d)", msg, ext1, ext2);
    switch (msg) {
        case MEDIA_PREPARED:
            pThis -> mCurrentState = MEDIA_PLAYER_PREPARED;
            break;
        case MEDIA_PLAYBACK_COMPLETE:
            pThis -> mCurrentState = MEDIA_PLAYER_PLAYBACK_COMPLETE;
            break;
    }

    pThis->mListener->notify(msg, ext1,ext2);
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

int FFMediaPlayer::setSurface(ANativeWindow* window) {
    ALOGE("FFMediaPlayer::setSurface");
    if (mPlayer != 0) {
        return mPlayer->setSurface(window);
    }

    return INVALID_OPERATION;
}

int FFMediaPlayer::start() {
    int ret = STATUS_OK;
    if (mCurrentState & MEDIA_PLAYER_STARTED) {
        ret = STATUS_OK;
    } else if ((mPlayer != 0) && (mCurrentState & (MEDIA_PLAYER_PREPARED | MEDIA_PLAYER_PLAYBACK_COMPLETE | MEDIA_PLAYER_PAUSED))) {
        mCurrentState = MEDIA_PLAYER_STARTED;
        ret = mPlayer->start();
        if (ret != STATUS_OK) {
            mCurrentState = MEDIA_PLAYER_STATE_ERROR;
        } else {
            if (mCurrentState == MEDIA_PLAYER_PLAYBACK_COMPLETE) {
                ALOGI("playback completed immediately following start()");
            }
        }
    } else {
        ret = INVALID_OPERATION;
    }
    return ret;
}

int FFMediaPlayer::stop() {
    int ret = STATUS_OK;
    ALOGE("FFMediaPlayer::stop %d", mCurrentState);
    if (mCurrentState & MEDIA_PLAYER_STOPPED) {
        ret = STATUS_OK;
    } else if ((mPlayer != 0) && (mCurrentState & (MEDIA_PLAYER_STARTED | MEDIA_PLAYER_PREPARED |
                                                   MEDIA_PLAYER_PAUSED | MEDIA_PLAYER_PLAYBACK_COMPLETE))) {

        ret = mPlayer->stop();
        if (ret != STATUS_OK) {
            mCurrentState = MEDIA_PLAYER_STATE_ERROR;
        } else {
            mCurrentState = MEDIA_PLAYER_STOPPED;
        }
    } else {
        ret = INVALID_OPERATION;
    }
    ALOGE("leaveFFMediaPlayer::stop %d", mCurrentState);
    return ret;
}

int FFMediaPlayer::pause() {
    ALOGE("FFMediaPlayer::pause()");
    if (mCurrentState & (MEDIA_PLAYER_PAUSED | MEDIA_PLAYER_PLAYBACK_COMPLETE)) {
        return STATUS_OK;
    }

    if ((mPlayer != 0) && (mCurrentState & MEDIA_PLAYER_STARTED)) {
        int ret = mPlayer->pause();
        if (ret != STATUS_OK) {
            mCurrentState = MEDIA_PLAYER_STATE_ERROR;
        } else {
            mCurrentState = MEDIA_PLAYER_PAUSED;
        }
        return ret;
    }

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

int FFMediaPlayer::reset() {
    if (mCurrentState == MEDIA_PLAYER_IDLE) {
        return STATUS_OK;
    }

    if (mPlayer != 0) {

        mPlayer->setNotifyCallback(0);

        int ret = mPlayer->reset();
        if (ret != STATUS_OK) {
            ALOGE("reset() failed with return code (%d)", ret);
            mCurrentState = MEDIA_PLAYER_STATE_ERROR;
        } else {
            mCurrentState = MEDIA_PLAYER_IDLE;
        }

        mPlayer = 0;

        return ret;
    }

    return STATUS_OK;
}






