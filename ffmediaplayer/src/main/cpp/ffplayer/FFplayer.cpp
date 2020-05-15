//
// Created by llm on 20-4-22.
//

#include "FFplayer.h"
#include "../macro.h"
#include "../FFLog.h"
#include "FFElement.h"
#include "DemuxElement.h"

/**
 * 准备线程pid_prepare真正执行的函数
 * @param args
 * @return
 */
void *task_prepare(void *args) {
    FFplayer *ffmpeg = static_cast<FFplayer *>(args);
    ffmpeg->prepare();
    return 0; //一定要return
}

FFplayer::FFplayer() {
    ALOGE("FFplayer contruct");
}

FFplayer::~FFplayer() {
    ALOGE("FFplayer decontruct");

    if (filePath != 0) {
        delete filePath;
        filePath = 0;
    }
}

void FFplayer::notify(int msg, int ext1, int ext2) {

}

int FFplayer::setDataSource(char * filePath) {
    this->filePath = new char[strlen(filePath) + 1];
    strcpy(this->filePath, filePath);

    return 0;
}

int FFplayer::prepare() {
    ALOGI("Enter %s", __FUNCTION__);

    avformatContext = avformat_alloc_context();
    int ret = 0;

    FFElement* demuxElement = new DemuxElement(filePath);
    ret = demuxElement->open(avformatContext, notify);
    if (ret != STATUS_OK) {
        return ret;
    }
}

int FFplayer::prepareAsync() {
    ALOGI("FFplayer::prepareAsync");
    pthread_create(&pid_prepare, 0, task_prepare, this);
    return 0;
}

int FFplayer::start() {
    return 0;
}

int FFplayer::stop() {
    return 0;
}

int FFplayer::pause() {
    return 0;
}

int FFplayer::reset() {
    return 0;
}

int FFplayer::release() {
    return 0;
}
