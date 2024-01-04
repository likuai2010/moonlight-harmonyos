//
// Created on 2024/1/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "AudrenAudioRenderer.h"

int AudrenAudioRenderer::init(int audio_configuration,
                              const POPUS_MULTISTREAM_CONFIGURATION opus_config,
                              void* context, int ar_flags) {
    m_channel_count = opus_config->channelCount;
    m_sample_rate = opus_config->sampleRate;
    m_samples_per_frame = opus_config->samplesPerFrame;
    
    m_buffer_size = m_latency * m_samples_per_frame * sizeof(short);
    m_samples = m_buffer_size / m_channel_count / sizeof(short);
    m_current_size = 0;

    m_decoded_buffer = (short*)malloc(m_channel_count * m_samples_per_frame * sizeof(short));

    int error;
    m_decoder = opus_multistream_decoder_create(
        opus_config->sampleRate, opus_config->channelCount,
        opus_config->streams, opus_config->coupledStreams, opus_config->mapping,
        &error);




    return DR_OK;
}
void AudrenAudioRenderer::cleanup() {

    if (m_decoder) {
        opus_multistream_decoder_destroy(m_decoder);
        m_decoder = nullptr;
    }

    if (m_decoded_buffer) {
        free(m_decoded_buffer);
        m_decoded_buffer = nullptr;
    }

    if (mempool_ptr) {
        free(mempool_ptr);
        mempool_ptr = nullptr;
    }

}

void AudrenAudioRenderer::decode_and_play_sample(char* data, int length) {
    if (m_decoder && m_decoded_buffer) {
        
        if (data != NULL && length > 0) {
            int decoded_samples = opus_multistream_decode(
                m_decoder, (const unsigned char*)data, length, m_decoded_buffer,
                m_samples_per_frame, 0);

            if (decoded_samples > 0) {
                write_audio(m_decoded_buffer,
                            decoded_samples * m_channel_count * sizeof(short));
            }
        }
    } else {
        //("Audren: Invalid call of decode_and_play_sample");
    }
}