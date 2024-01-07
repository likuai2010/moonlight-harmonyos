//
// Created by Gu,Xiuzhong on 2021/4/13.
//

#ifndef PCMPLAY_AUDIO_H
#define PCMPLAY_AUDIO_H

#include "DataQueue.h"
#include <pthread.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_OpenHarmony.h>
#include <time.h>
#include "hilog/log.h"
#define LOGD(...)  OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "testTag", __VA_ARGS__);
#define LOGE(...)  OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "testTag", __VA_ARGS__);

class Audio {

public:
    DataQueue *dataQueue;
    int sample_rate = 0;
    pthread_t play_thread_t;

    SLOHBufferQueueItf pcmBufferQueue = NULL;
    SLObjectItf engineObject= NULL;
    SLEngineItf engineEngine= NULL;
    SLObjectItf pcmPlayerObject = NULL;
    SLObjectItf outputMixObject = NULL;
    SLEnvironmentalReverbItf outputMixEnvironmentalReverb = NULL;
    SLPlayItf pcmPlayerPlay = NULL;
    SLMuteSoloItf pcmMutePlay = NULL;
    SLVolumeItf pcmVolumePlay = NULL;

public:
    Audio(DataQueue *dataQueue ,int sample_rate);

    ~Audio();

    void play();

    void initOpenSLES();

    int getCurrentSampleRateForOpensles(int sample_rate);

    void resume();

    void pause();

    void release();

    void stop();
};


#endif //PCMPLAY_AUDIO_H
