//
// Created on 2024/1/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_harmonyos_AudrenAudioRenderer_H
#define moonlight_harmonyos_AudrenAudioRenderer_H

#include <Limelight.h>
#include <opus_multistream.h>
#include "cstddef"
#include "IAudioRenderer.h"

#include <inttypes.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_COUNT 5

class AudrenAudioRenderer : public IAudioRenderer {
  public:
    AudrenAudioRenderer(){};
    ~AudrenAudioRenderer(){};

    int init(int audio_configuration,
             const POPUS_MULTISTREAM_CONFIGURATION opus_config, void* context,
             int ar_flags) override;
    void cleanup() override;
    void decode_and_play_sample(char* sample_data, int sample_length) override;
    int capabilities() override;

  private:
    size_t free_wavebuf_index();
    size_t append_audio(const void* buf, size_t size);
    void write_audio(const void* buf, size_t size);
    bool flush();

    OpusMSDecoder* m_decoder = nullptr;
    short* m_decoded_buffer = nullptr;
    void* mempool_ptr = nullptr;
    void* current_pool_ptr = nullptr;


    bool m_inited_driver = false;
    int m_channel_count = 0;
    int m_samples_per_frame = 1;
    int m_sample_rate = 0;
    int m_buffer_size = 0;
    int m_samples = 0;
    size_t m_total_queued_samples = 0;
    size_t m_current_size = 0;

    const int m_latency = 5;
};


#endif //moonlight-harmonyos_AudrenAudioRenderer_H
