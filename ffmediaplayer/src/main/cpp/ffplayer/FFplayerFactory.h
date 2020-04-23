//
// Created by llm on 20-4-22.
//

#ifndef FFMEDIAPLAYER_FFPLAYERFACTORY_H
#define FFMEDIAPLAYER_FFPLAYERFACTORY_H

#include "../FactoryInterface.h"

class FFplayerFactory : public FactoryInterface {
public:
    std::shared_ptr<MediaPlayerInterface> createPlayer();
};

#endif //FFMEDIAPLAYER_FFPLAYERFACTORY_H
