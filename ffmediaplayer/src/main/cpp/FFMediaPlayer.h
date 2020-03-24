//
// Created by yizho on 2020/3/23.
//

#ifndef FFMEDIAPLAYER_FFMEDIAPLAYER_H
#define FFMEDIAPLAYER_FFMEDIAPLAYER_H

#include "MediaPlayerListener.h"

class FFMediaPlayer {
public:
    FFMediaPlayer();
    ~FFMediaPlayer();

    void setListener(MediaPlayerListener *listener);
    void testCallback(bool bNewThread);

private:
    MediaPlayerListener  *mListener = 0;
};

#endif //FFMEDIAPLAYER_FFMEDIAPLAYER_H
