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

    /*
     * source pad 通过该函数往sink pad 传递数据
     * */
    void notify(void* data) {
        if (mObserver) {
            mObserver->addData(data);
        }
    }

    IObserver* getObserver() {
        return mObserver;
    }

    virtual void addData(void* data) = 0;
    virtual void setRecivieData(bool flag) = 0;

protected:

    IObserver* mObserver = 0;
};


#endif //FFMEDIAPLAYER_IOBSERVER_H
