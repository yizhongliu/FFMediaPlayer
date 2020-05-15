//
// Created by llm on 20-5-15.
//

#ifndef FFMEDIAPLAYER_IOBSERVER_H
#define FFMEDIAPLAYER_IOBSERVER_H


class IObserver
{
public:

    void setObserver(IObserver *obs) {
        mObserver = obs;
    }

    void notify(void* data) {
        update(data);
    }

    virtual void update(void* data) = 0;

protected:

    IObserver* mObserver = 0;
};


#endif //FFMEDIAPLAYER_IOBSERVER_H
