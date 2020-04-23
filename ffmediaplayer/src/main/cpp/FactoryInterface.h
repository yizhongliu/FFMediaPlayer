//
// Created by llm on 20-4-22.
//

#ifndef FFMEDIAPLAYER_MEDIAPLAYERFACTORY_H
#define FFMEDIAPLAYER_MEDIAPLAYERFACTORY_H

#include "MediaPlayerInterface.h"
#include <memory>

class FactoryInterface {
public:
    virtual ~FactoryInterface() {}
    virtual std::shared_ptr<MediaPlayerInterface> createPlayer() = 0;
};

#endif //FFMEDIAPLAYER_MEDIAPLAYERFACTORY_H
