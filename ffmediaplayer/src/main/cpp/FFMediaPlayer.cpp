//
// Created by yizho on 2020/3/23.
//

#include <pthread.h>
#include "FFMediaPlayer.h"

FFMediaPlayer::FFMediaPlayer() {

}

FFMediaPlayer::~FFMediaPlayer() {

}

void FFMediaPlayer::setListener(MediaPlayerListener *listener) {
    mListener = listener;
}

void *task_test(void *args) {
    MediaPlayerListener *listener = static_cast<MediaPlayerListener *>(args);
    listener->notify(2,2,2);
    return 0;//一定一定一定要返回0！！！
}

void FFMediaPlayer::testCallback(bool newThread) {
    if (newThread) {
        pthread_t pid_test;
        pthread_create(&pid_test, 0, task_test, mListener);
    } else {
        mListener->notify(1,1,1);
    }

}



