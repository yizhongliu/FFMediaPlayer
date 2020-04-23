//
// Created by llm on 20-4-22.
//

#include "FFplayerFactory.h"
#include "FFplayer.h"
#include <memory>

std::shared_ptr<MediaPlayerInterface> FFplayerFactory::createPlayer() {
   return std::shared_ptr<MediaPlayerInterface>(new FFplayer());
}