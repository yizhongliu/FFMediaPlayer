//
// Created by llm on 20-3-24.
//

#ifndef FFMEDIAPLAYER_MEDIAPLAYERLISTENER_H
#define FFMEDIAPLAYER_MEDIAPLAYERLISTENER_H
class MediaPlayerListener {
public:
    virtual void notify(int msg, int ext1, int ext2) = 0;
};
#endif //FFMEDIAPLAYER_MEDIAPLAYERLISTENER_H
