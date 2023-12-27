//
// Created on 2023/12/23.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_eglRender_H
#define moonlight_eglRender_H
#include "video/render/IFFmpegRender.h"

class EglVideoRenderer : public IFFmpegRender {
  public:
    explicit EglVideoRenderer(){};
    ~EglVideoRenderer();
    virtual bool initialize(DECODER_PARAMETERS *params) override;
    virtual void renderFrame(AVFrame *frame) override;
    void Release(){};
    GLuint m_texture_id[3] = {0, 0, 0};
    EGLDisplay m_eglDisplay = EGL_NO_DISPLAY;
    EGLSurface m_eglSurface = EGL_NO_SURFACE;

  private:
    void checkAndUpdateScale(AVFrame *frame);
    void bindTexture(int id);

    EGLNativeWindowType m_eglWindow;

    EGLConfig m_eglConfig = EGL_NO_CONFIG_KHR;

    EGLContext m_eglContext = EGL_NO_CONTEXT;
    GLuint m_program;

    GLuint m_vbo, m_vao;

    GLint m_texture_uniform[3];
    int m_yuvmat_location;
    int m_offset_location;
    int m_uv_data_location;
    int textureWidth[3];
    int textureHeight[3];
    int m_frame_width = 0;
    int m_frame_height = 0;
    float borderColor[3] = {0.0f, 0.5f, 0.5f};
    
    bool m_flag = false;
    int m_width;
    int m_height;
    GLfloat m_widthPercent;
};

#endif // moonlight_eglRender_H
