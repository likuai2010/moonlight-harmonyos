//
// Created on 2024/1/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_harmonyos_IAudioRenderer_H
#define moonlight_harmonyos_IAudioRenderer_H
extern "C" {
#include <Limelight.h>
}
#pragma once

class IAudioRenderer {
  public:
    virtual ~IAudioRenderer(){};
    virtual int init(int audio_configuration,
                     const POPUS_MULTISTREAM_CONFIGURATION opus_config,
                     void* context, int ar_flags) = 0;
    virtual void start(){};
    virtual void stop(){};
    virtual void cleanup() = 0;
    virtual void decode_and_play_sample(char* sample_data,
                                        int sample_length) = 0;
    virtual int capabilities() = 0;
};

#endif //moonlight-harmonyos_IAudioRenderer_H
