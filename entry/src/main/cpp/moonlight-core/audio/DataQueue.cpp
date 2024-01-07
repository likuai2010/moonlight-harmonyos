//
// Created by Gu,Xiuzhong on 2021/4/13.
//

#include "DataQueue.h"

DataQueue::DataQueue() {
    pthread_mutex_init(&mutexPacket, NULL);
    pthread_cond_init(&condPacket, NULL);
}

DataQueue::~DataQueue() {
    pthread_mutex_destroy(&mutexPacket);
    pthread_cond_destroy(&condPacket);
}

int DataQueue::putPcmData(PcmData *data) {

    pthread_mutex_lock(&mutexPacket);
    queuePacket.push(data);
    pthread_cond_signal(&condPacket);
    pthread_mutex_unlock(&mutexPacket);

    return 0;
}

PcmData *DataQueue::getPcmData() {
    pthread_mutex_lock(&mutexPacket);
    PcmData *pkt = NULL;
    if (queuePacket.size() > 0) {
        pkt = queuePacket.front();
        queuePacket.pop();
    } else {
        pthread_cond_wait(&condPacket, &mutexPacket);
    }
    pthread_mutex_unlock(&mutexPacket);
    return pkt;
}

int DataQueue::clearPcmData() {

    pthread_cond_signal(&condPacket);
    pthread_mutex_lock(&mutexPacket);
    while (!queuePacket.empty()) {
        PcmData *pkt = queuePacket.front();
        queuePacket.pop();
        free(pkt->getData());
        pkt = NULL;
    }
    pthread_mutex_unlock(&mutexPacket);
    return 0;
}

void DataQueue::release() {
    clearPcmData();
}

int DataQueue::getPcmDataSize() {
    int size = 0;
    pthread_mutex_lock(&mutexPacket);
    size = queuePacket.size();
    pthread_mutex_unlock(&mutexPacket);
    return size;
}

