//
// Created by Gu,Xiuzhong on 2021/4/13.
//

#include "Audio.h"
#include "sys/time.h"
#include <stdint.h>

Audio::Audio(DataQueue *dataQueue, int sample_rate) {
    this->dataQueue = dataQueue;
    this->sample_rate = sample_rate;
}

Audio::~Audio() {
    release();
}

void *p_initOpenSLES(void *data) {
    Audio *audio = static_cast<Audio *>(data);
    audio->initOpenSLES();
    pthread_exit(&audio->play_thread_t);
}

void Audio::play() {
    pthread_create(&play_thread_t, nullptr, p_initOpenSLES, this);
}

// OpenSLES 会自动回调
void pcmBufferCallBack(SLOHBufferQueueItf bf, void *context, SLuint32 size) {
//    LOGD("pcmBufferCallBack ok");

    Audio *audio = (Audio *) context;
    if (audio != NULL) {
        PcmData *data = audio->dataQueue->getPcmData();
        if (NULL != data) {
            LOGD("Enqueue ok");
            (*audio->pcmBufferQueue)->Enqueue(audio->pcmBufferQueue,
                                              data->getData(),
                                              data->getSize());
        }
    }
}

void Audio::initOpenSLES() {
    struct timeval t_start, t_end;
    gettimeofday(&t_start, NULL);
    LOGD("Start time: %ld us", t_start.tv_usec);

    /***********  1 创建引擎 获取SLEngineItf***************/
    SLresult result;
    result = slCreateEngine(&engineObject, 0, 0, 0, 0, 0);
    if (result != SL_RESULT_SUCCESS)
        return;
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS)
        return;
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
    if (result != SL_RESULT_SUCCESS)
        return;
    if (engineEngine) {
        LOGD("get SLEngineItf success");
    } else {
        LOGE("get SLEngineItf failed");
    }
    /***********         1 创建引擎       ***************/


    /***********  2 创建混音器 ***************/

    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, mids, mreq);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("CreateOutputMix failed");
        return;
    } else {
        LOGD("CreateOutputMix success");
    }

    //实例化混音器
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("mixer init failed");
    } else {
        LOGD("mixer init success");
    }

    result = (*outputMixObject)->GetInterface(outputMixObject, SL_IID_ENVIRONMENTALREVERB,
                                              &outputMixEnvironmentalReverb);
    if (SL_RESULT_SUCCESS == result) {
        SLEnvironmentalReverbSettings reverbSettings = SL_I3DL2_ENVIRONMENT_PRESET_STONECORRIDOR;
        result = (*outputMixEnvironmentalReverb)->SetEnvironmentalReverbProperties(
                outputMixEnvironmentalReverb, &reverbSettings);
        (void) result;
    }

    /***********  2 创建混音器 ***************/

    /***********  3 配置音频信息 ***************/
    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink slDataSink = {&outputMix, 0};
    //缓冲队列
    SLDataLocator_BufferQueue android_queue = {SL_DATALOCATOR_BUFFERQUEUE,
                                                            2};
    //音频格式
    SLDataFormat_PCM pcmFormat = {
            SL_DATAFORMAT_PCM, //播放pcm格式的数据
            2,   //声道数
            static_cast<SLuint32>(getCurrentSampleRateForOpensles(sample_rate)),
            SL_PCMSAMPLEFORMAT_FIXED_16, //位数 16位
            SL_PCMSAMPLEFORMAT_FIXED_16, //和位数一致就行
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT, //立体声（前左前右）
            //字节序，小端
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource slDataSource = {&android_queue, &pcmFormat};
    /***********  3 配置音频信息 ***************/

    /************* 4 创建播放器 ****************/
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME, SL_IID_MUTESOLO};
    const SLboolean req[] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};


    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &pcmPlayerObject, &slDataSource,
                                                &slDataSink, sizeof(ids) / sizeof(SLInterfaceID),
                                                ids, req);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("create audio player failed");
    } else {
        LOGE("create audio player success");
    }
    //初始化播放器
    result = (*pcmPlayerObject)->Realize(pcmPlayerObject, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("audio player init failed");
    } else {
        LOGE("audio player init success");
    }
    //获取player接口
    result = (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_PLAY, &pcmPlayerPlay);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("player get SL_IID_PLAY failed");
    } else {
        LOGD("player get SL_IID_PLAY success");
    }
    //    获取声道操作接口
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_MUTESOLO, &pcmMutePlay);
    // 音量
    (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_VOLUME, &pcmVolumePlay);

    //获取播放队列接口
    result = (*pcmPlayerObject)->GetInterface(pcmPlayerObject, SL_IID_OH_BUFFERQUEUE, &pcmBufferQueue);
    if (result != SL_RESULT_SUCCESS) {
        LOGE("player get SL_IID_BUFFERQUEUE failed");
    } else {
        LOGD("player get SL_IID_BUFFERQUEUE success");
    }
    /************* 4 创建播放器 ****************/

    //设置回调函数
    (*pcmBufferQueue)->RegisterCallback(pcmBufferQueue, pcmBufferCallBack, this);
    //设置播放状态
    (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    // 启动
    pcmBufferCallBack(pcmBufferQueue, this, 0);

    // 启动
//    (*pcmBufferQueue)->Enqueue(pcmBufferQueue,"",1);


    gettimeofday(&t_end, NULL);
    LOGD("End time: %ld us", t_end.tv_usec);

    long cost_time = t_end.tv_usec - t_start.tv_usec;
    LOGD("opensled create cost:%ld ms", cost_time / 1000);
}

int Audio::getCurrentSampleRateForOpensles(int sample_rate) {
    int rate = 0;
    switch (sample_rate) {
        case 8000:
            rate = SL_SAMPLINGRATE_8;
            break;
        case 11025:
            rate = SL_SAMPLINGRATE_11_025;
            break;
        case 12000:
            rate = SL_SAMPLINGRATE_12;
            break;
        case 16000:
            rate = SL_SAMPLINGRATE_16;
            break;
        case 22050:
            rate = SL_SAMPLINGRATE_22_05;
            break;
        case 24000:
            rate = SL_SAMPLINGRATE_24;
            break;
        case 32000:
            rate = SL_SAMPLINGRATE_32;
            break;
        case 44100:
            rate = SL_SAMPLINGRATE_44_1;
            break;
        case 48000:
            rate = SL_SAMPLINGRATE_48;
            break;
        case 64000:
            rate = SL_SAMPLINGRATE_64;
            break;
        case 88200:
            rate = SL_SAMPLINGRATE_88_2;
            break;
        case 96000:
            rate = SL_SAMPLINGRATE_96;
            break;
        case 192000:
            rate = SL_SAMPLINGRATE_192;
            break;
        default:
            rate = SL_SAMPLINGRATE_44_1;
    }
    LOGD("rate %d", rate);
    return rate;
}


void Audio::resume() {
    if (pcmPlayerPlay != nullptr) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PLAYING);
    }
}

void Audio::pause() {
    if (pcmPlayerPlay != nullptr) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_PAUSED);
    }
}

void Audio::stop() {
    if (pcmPlayerPlay != nullptr) {
        (*pcmPlayerPlay)->SetPlayState(pcmPlayerPlay, SL_PLAYSTATE_STOPPED);
    }
}

void Audio::release() {
    LOGD("start release");
//    pause();
    stop();

    if (pcmPlayerObject != NULL) {
        LOGD("start Destroy");
        // 这个歌方法特别耗时？
//        (*pcmPlayerObject)->Destroy(pcmPlayerObject);

        pcmPlayerObject = NULL;
        pcmPlayerPlay = NULL;
        pcmBufferQueue = NULL;
    }
    LOGD("pcmPlayerObject  release");
    if (outputMixObject != NULL) {
        (*outputMixObject)->Destroy(outputMixObject);
        outputMixObject = NULL;
        outputMixEnvironmentalReverb = NULL;
    }
    LOGD("outputMixObject  release");

    if (engineObject != NULL) {
        (*engineObject)->Destroy(engineObject);
        engineObject = NULL;
        engineEngine = NULL;
    }
    LOGD("engineObject  release");
}

