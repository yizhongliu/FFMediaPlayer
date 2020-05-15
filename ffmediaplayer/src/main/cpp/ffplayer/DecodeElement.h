//
// Created by llm on 20-5-14.
//

#ifndef FFMEDIAPLAYER_DECODEELEMENT_H
#define FFMEDIAPLAYER_DECODEELEMENT_H

#include "FFElement.h"

class DecodeElement: public FFElement {
public:
    DecodeElement(const char* filepath);
    ~DecodeElement();

    virtual int    open(AVFormatContext * avFormatContext, notify_callback_f notifyFunc);
    virtual int    start();
    virtual int    pause();
    virtual int    stop();
    virtual int    release();
    virtual int    reset();


private:
    char* filePath = 0;
};

#endif //FFMEDIAPLAYER_DECODEELEMENT_H
