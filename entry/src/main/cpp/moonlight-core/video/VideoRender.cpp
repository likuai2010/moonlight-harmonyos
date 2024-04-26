//
// Created on 2024/4/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "VideoRender.h"
#include "decoder/NativeVideoDecoder.h"
#include "AVFrameHolder.h"
#include <unistd.h>

VideoRender::VideoRender()  {
    // 解码器适配层
    #ifdef FFMPEG_ENABLED
        m_decoder = new FFmpegVideoDecoder();
    #endif
    if (NativeVideoDecoder::supportedHW()){
        m_decoder = new NativeVideoDecoder();
    }
    m_eglRender = new EglVideoRenderer();
}

int VideoRender::setup(DECODER_PARAMETERS params){
    m_decode_params = &params;
   
    return m_decoder->setup(params);
}
int VideoRender::submitDecodeUnit(PDECODE_UNIT du){
    return m_decoder->submitDecodeUnit(du);
}

void VideoRender::startRenderFrame(){
    if(m_decode_params != NULL){
        m_eglRender->initialize(getParams());
        running = true;
        while (running) {
            AVFrameHolder::GetInstance()->get([this](AVFrame *frame) { m_eglRender->renderFrame(frame); });
            usleep(100000 / 120);
        }
    }
}
void *renderFrame(void *data) {
    VideoRender *audio = static_cast<VideoRender *>(data);
    audio->startRenderFrame();
}
DECODER_PARAMETERS* VideoRender::getParams(){
    return m_decoder->getParams();
}
void VideoRender::start(){
    m_decoder->start();
    //pthread_create(&render_thread_t, nullptr, renderFrame, this);
}

void VideoRender::stop(){
    m_decoder->stop();
    running = false;
    //pthread_exit(&render_thread_t);
}
VIDEO_STATS* VideoRender::video_decode_stats(){
    return m_decoder->video_decode_stats();
}
void VideoRender::cleanup(){
    //m_decoder->cleanup();
}

