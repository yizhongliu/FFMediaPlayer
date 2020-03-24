//
// Created by yizho on 2020/3/24.
//

#ifndef FFMEDIAPLAYER_CMUTEX_H
#define FFMEDIAPLAYER_CMUTEX_H
#include <pthread.h>

class CMutex {
public:
    CMutex();
    ~CMutex();

    void Lock();
    void Unlock();

private:
    pthread_mutex_t mutex;
};

class CAutoLock {
public:
    CAutoLock(CMutex &);
    ~CAutoLock();

private:
    CMutex& mLock;
};
#endif //FFMEDIAPLAYER_CMUTEX_H
