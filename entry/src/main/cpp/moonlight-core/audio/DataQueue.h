//
// Created by Gu,Xiuzhong on 2021/4/13.
//

#ifndef PCMPLAY_DATAQUEUE_H
#define PCMPLAY_DATAQUEUE_H

#include "queue"
#include "PcmData.h"
#include "pthread.h"

class DataQueue {
public:
    std::queue<PcmData *> queuePacket;
    pthread_mutex_t mutexPacket;
    pthread_cond_t condPacket;
public:
    DataQueue();

    ~DataQueue();

    int putPcmData(PcmData *data);

    PcmData *getPcmData();

    int getPcmDataSize();

    int clearPcmData();

    void release();
};


#endif //PCMPLAY_DATAQUEUE_H
