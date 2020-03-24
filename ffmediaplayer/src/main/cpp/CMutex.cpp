//
// Created by yizho on 2020/3/24.
//

#include "CMutex.h"

CMutex::CMutex() {
    pthread_mutex_init(&mutex, NULL);
}

CMutex::~CMutex() {
    pthread_mutex_destroy(&mutex);
}

void CMutex::Lock(){
    pthread_mutex_lock(&mutex);
}

void CMutex::Unlock(){
    pthread_mutex_unlock(&mutex);
}

CAutoLock::CAutoLock(CMutex& m) : mLock(m) {
    mLock.Lock();
}

CAutoLock::~CAutoLock() {
    mLock.Unlock();
}