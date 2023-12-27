//
// Created on 2023/12/23.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_IFFmpegRender_H
#define moonlight_IFFmpegRender_H
#include "video/FFmpegVideoDecoder.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
class IFFmpegRender{
    virtual bool initialize(DECODER_PARAMETERS* params) = 0;
    virtual void renderFrame(AVFrame* frame) = 0;
};
#endif //moonlight_IFFmpegRender_H
