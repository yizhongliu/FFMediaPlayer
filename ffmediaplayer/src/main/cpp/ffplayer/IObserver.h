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

    bool hasObserver () {
        if (mObserver) {
            return true;
        } else {
            return false;
        }
    }

    void notify(void* data) {
        if (mObserver) {
            mObserver->addData(data);
        }
    }

    virtual void addData(void* data) = 0;

protected:

    IObserver* mObserver = 0;
};


#endif //FFMEDIAPLAYER_IOBSERVER_H
