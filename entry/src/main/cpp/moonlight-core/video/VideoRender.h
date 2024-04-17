//
// Created on 2024/4/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_VideoRender_H
#define moonlight_VideoRender_H
#include "decoder/decode.h"
#include "video/render/plugin_render.h"

class VideoRender: public IVideoDecoder {
  public:
    explicit VideoRender();
    ~VideoRender(){};
    int setup(DECODER_PARAMETERS params) override;
    void start() override;
    void stop() override;
    void cleanup() override;
    int submitDecodeUnit(PDECODE_UNIT du) override;
    VIDEO_STATS* video_decode_stats() override;
    DECODER_PARAMETERS *getParams() override;
    void startRenderFrame();
  private:
    IVideoDecoder *m_decoder;
    EglVideoRenderer *m_eglRender;
    pthread_t render_thread_t;
    DECODER_PARAMETERS* m_decode_params;
};
#endif //moonlight_VideoRender_H
