//
// Created on 2024/1/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "SDLAudioRenderer.h"
#include <algorithm>

SDLAudioRenderer::SDLAudioRenderer(){
    
}
int SDLAudioRenderer::init(int audio_configuration,
                              const POPUS_MULTISTREAM_CONFIGURATION opus_config,
                              void* context, int ar_flags) {
    int rc;
    decoder = opus_multistream_decoder_create(
        opus_config->sampleRate, opus_config->channelCount,
        opus_config->streams, opus_config->coupledStreams, opus_config->mapping,
        &rc);

    channelCount = opus_config->channelCount;

    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = opus_config->sampleRate;
    want.format = SDL_AUDIO_S16LE;
    want.channels = opus_config->channelCount;
    
    // SDL 3.0
    m_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_OUTPUT, &want, NULL, NULL);
    SDL_ResumeAudioDevice(SDL_GetAudioStreamDevice(m_stream));
    //    dev = SDL_OpenAudioDevice(devices[0], &want);
    //    if (dev == 0) {
    //        SDL_Log("Failed to open audio: %s\n", SDL_GetError());
    //        return -1;
    //    } else {
    //        if (have.format != want.format) // we let this one thing change.
    //            SDL_Log("We didn't get requested audio format.\n");
    //        SDL_PauseAudioDevice(dev); // start audio playing.
    //    }
    return DR_OK;
}
void SDLAudioRenderer::cleanup() {
     if (decoder != NULL)
        opus_multistream_decoder_destroy(decoder);

     //SDL_CloseAudioDevice(dev);
     SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SDLAudioRenderer::decode_and_play_sample(char *data, int length) {
    int decodeLen =
            opus_multistream_decode(decoder, (const unsigned char*)data,
                                    length, pcmBuffer, FRAME_SIZE, 0);
    int volume = 10;
    for (int i = 0; i < FRAME_SIZE * MAX_CHANNEL_COUNT; i++) {
        int scale = pcmBuffer[i] * (volume / 100.0);
        pcmBuffer[i] = std::min(SHRT_MAX, std::max(SHRT_MIN, scale));
    }
    if (decodeLen > 0) {
        SDL_PutAudioStreamData(m_stream, pcmBuffer, decodeLen);
//        if (SDL_GetQueuedAudioSize(dev) > 16000) {
//            // clear audio queue to avoid big audio delay
//            // average values are close to 16000 bytes
//            SDL_ClearQueuedAudio(this->dev);
//        }
//
//        SDL_QueueAudio(dev, pcmBuffer,
//                       decodeLen * channelCount * sizeof(short));
    } else {
        SDL_Log("Opus error from decode: %d\n", decodeLen);
    }
}
int SDLAudioRenderer::capabilities() { return CAPABILITY_DIRECT_SUBMIT; }