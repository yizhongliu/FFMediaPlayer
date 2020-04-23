//
// Created by yizho on 2020/3/23.
//

#ifndef FFMEDIAPLAYER_FFMEDIAPLAYER_H
#define FFMEDIAPLAYER_FFMEDIAPLAYER_H

#include "MediaPlayerListener.h"
#include "MediaPlayerInterface.h"

enum media_player_states {
    MEDIA_PLAYER_STATE_ERROR        = 0,
    MEDIA_PLAYER_IDLE               = 1 << 0,
    MEDIA_PLAYER_INITIALIZED        = 1 << 1,
    MEDIA_PLAYER_PREPARING          = 1 << 2,
    MEDIA_PLAYER_PREPARED           = 1 << 3,
    MEDIA_PLAYER_STARTED            = 1 << 4,
    MEDIA_PLAYER_PAUSED             = 1 << 5,
    MEDIA_PLAYER_STOPPED            = 1 << 6,
    MEDIA_PLAYER_PLAYBACK_COMPLETE  = 1 << 7
};

class FFMediaPlayer {
public:
    FFMediaPlayer();
    ~FFMediaPlayer();

    void setListener(MediaPlayerListener *listener);

    //TODO: remove just for test
    void testCallback(bool bNewThread);
    void testCreatePlayer();

private:
    MediaPlayerListener  *mListener = 0;
    media_player_states mCurrentState;

    //真正的播放器功能实现类
    std::shared_ptr<MediaPlayerInterface> mPlayer;
};

#endif //FFMEDIAPLAYER_FFMEDIAPLAYER_H
