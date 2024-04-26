//
// Created on 2024/1/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_harmonyos_SDLAudioRenderer_H
#define moonlight_harmonyos_SDLAudioRenderer_H

#include <Limelight.h>
#include <opus_multistream.h>
#include "IAudioRenderer.h"

#include <malloc.h>

#define BUFFER_COUNT 5
#define MAX_CHANNEL_COUNT 6
#define FRAME_SIZE 240
#define FRAME_BUFFER 12
#include "Audio.h"

class SDLAudioRenderer : public IAudioRenderer {
  public:
    explicit SDLAudioRenderer();
    ~SDLAudioRenderer(){};

    int init(int audio_configuration,
             const POPUS_MULTISTREAM_CONFIGURATION opus_config, void *context,
             int ar_flags) override;
    void cleanup() override;
    void decode_and_play_sample(char *sample_data, int sample_length) override;
    int capabilities() override;

  private:
    DataQueue *dataQueue = NULL;
    Audio *audio = NULL;
    OpusMSDecoder *decoder;
    short pcmBuffer[FRAME_SIZE * MAX_CHANNEL_COUNT];
    int channelCount;
};

#endif // moonlight-harmonyos_SDLAudioRenderer_H
