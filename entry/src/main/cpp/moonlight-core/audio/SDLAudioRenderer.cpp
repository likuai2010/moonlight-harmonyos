//
// Created on 2024/1/4.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "SDLAudioRenderer.h"
#include <algorithm>
#include <unistd.h>

SDLAudioRenderer::SDLAudioRenderer() {
}
int SDLAudioRenderer::init(int audio_configuration,
                           const POPUS_MULTISTREAM_CONFIGURATION opus_config,
                           void *context, int ar_flags) {
     int rc;
        decoder = opus_multistream_decoder_create(
            opus_config->sampleRate, opus_config->channelCount,
            opus_config->streams, opus_config->coupledStreams, opus_config->mapping,
            &rc);
        channelCount = opus_config->channelCount;
     if(dataQueue == NULL){
        dataQueue = new DataQueue();
    }
     if (audio == NULL) {
        audio = new Audio(dataQueue, 48000);
        audio->play();
     }
   
   /*  SDL_InitSubSystem(SDL_INIT_AUDIO);
     SDL_AudioSpec want;
     SDL_zero(want);
     want.freq = 48000;
     want.format = SDL_AUDIO_S16;
     want.channels = 2;

     // SDL 3.0
     m_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_OUTPUT, &want, NULL, NULL);
     SDL_AudioDeviceID deviceId = SDL_GetAudioStreamDevice(m_stream);
     return SDL_ResumeAudioDevice(deviceId);*/
    return 0;
}
void SDLAudioRenderer::cleanup() {
    if (decoder != NULL)
        opus_multistream_decoder_destroy(decoder);
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SDLAudioRenderer::decode_and_play_sample(char *data, int length) {
        int decodeLen =
                opus_multistream_decode(decoder, (const unsigned char*)data,
                                        length, pcmBuffer, FRAME_SIZE, 0);
    //    int volume = 80;
    //    for (int i = 0; i < FRAME_SIZE * MAX_CHANNEL_COUNT; i++) {
    //        int scale = pcmBuffer[i] * (volume / 100.0);
    //        pcmBuffer[i] = std::min(SHRT_MAX, std::max(SHRT_MIN, scale));
    //    }
    if (length > 0) {
        // decodeLen * channelCount * sizeof(short)
          PcmData *pdata = new PcmData((char *)(pcmBuffer), decodeLen * channelCount * sizeof(short));
            dataQueue->putPcmData(pdata);
        //SDL_PutAudioStreamData(m_stream, data, length);
    } else {
        SDL_Log("Opus error from decode: %d\n", length);
    }
}
int SDLAudioRenderer::capabilities() { return CAPABILITY_DIRECT_SUBMIT; }