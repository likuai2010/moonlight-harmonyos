//
// Created on 2023/12/23.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "video/render/eglRender.h"
#include "hilog/log.h"
#include "video/common/common.h"
#include "Shader.h"

#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#define eglLog(level, ...) OH_LOG_Print(LOG_APP, level, LOG_DOMAIN, "EglCore", __VA_ARGS__)

static const char *fragYUV420P =
        "#version 300 es\n"

        "precision mediump float;\n"
        "//纹理坐标\n"
        "in vec2 vTextCoord;\n"
        "//输入的yuv三个纹理\n"
        "uniform sampler2D yTexture;//采样器\n"
        "uniform sampler2D uTexture;//采样器\n"
        "uniform sampler2D vTexture;//采样器\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "//采样到的yuv向量数据\n"
        "   vec3 yuv;\n"
        "//yuv转化得到的rgb向量数据\n"
        "    vec3 rgb;\n"
        "    //分别取yuv各个分量的采样纹理\n"
        "    yuv.x = texture(yTexture, vTextCoord).r;\n"
        "   yuv.y = texture(uTexture, vTextCoord).g - 0.5;\n"
        "    yuv.z = texture(vTexture, vTextCoord).b - 0.5;\n"
        "   rgb = mat3(\n"
        "            1.0, 1.0, 1.0,\n"
        "            0.0, -0.183, 1.816,\n"
        "            1.540, -0.459, 0.0\n"
        "    ) * yuv;\n"
        "    //gl_FragColor是OpenGL内置的\n"
        "    FragColor = vec4(rgb, 1.0);\n"
        " }";

static const char *vertexShaderWithMatrix =
        "        #version 300 es\n"
        "        layout (location = 0) \n"
        "        in vec4 aPosition;//输入的顶点坐标，会在程序指定将数据输入到该字段\n"//如果传入的向量是不够4维的，自动将前三个分量设置为0.0，最后一个分量设置为1.0

        "        layout (location = 1) \n"
        "        in vec2 aTextCoord;//输入的纹理坐标，会在程序指定将数据输入到该字段\n"
        "\n"
        "        out\n"
        "        vec2 vTextCoord;//输出的纹理坐标;\n"
        "        uniform mat4 uMatrix;"//变换矩阵
        "\n"
        "        void main() {\n"
        "            //这里其实是将上下翻转过来（因为安卓图片会自动上下翻转，所以转回来）\n"
        "             vTextCoord = vec2(aTextCoord.x, 1.0 - aTextCoord.y);\n"
        "            //直接把传入的坐标值作为传入渲染管线。gl_Position是OpenGL内置的\n"
        //    "            gl_Position = aPosition;\n"
        "            gl_Position = aPosition;\n"
        "        }";

static const char *texture_mappings[] = {"ymap", "umap", "vmap"};
static const float vertices[] = {-1.0f, -1.0f, 1.0f, -1.0f,
                                 -1.0f, 1.0f, 1.0f, 1.0f};

static const float *gl_color_offset(bool color_full) {
    static const float limitedOffsets[] = {16.0f / 255.0f, 128.0f / 255.0f,
                                           128.0f / 255.0f};
    static const float fullOffsets[] = {0.0f, 128.0f / 255.0f, 128.0f / 255.0f};
    return color_full ? fullOffsets : limitedOffsets;
}
static const float *gl_color_matrix(enum AVColorSpace color_space,
                                    bool color_full) {
    static const float bt601Lim[] = {1.1644f, 1.1644f, 1.1644f, 0.0f, -0.3917f,
                                     2.0172f, 1.5960f, -0.8129f, 0.0f};
    static const float bt601Full[] = {
        1.0f, 1.0f, 1.0f, 0.0f, -0.3441f, 1.7720f, 1.4020f, -0.7141f, 0.0f};
    static const float bt709Lim[] = {1.1644f, 1.1644f, 1.1644f, 0.0f, -0.2132f,
                                     2.1124f, 1.7927f, -0.5329f, 0.0f};
    static const float bt709Full[] = {
        1.0f, 1.0f, 1.0f, 0.0f, -0.1873f, 1.8556f, 1.5748f, -0.4681f, 0.0f};
    static const float bt2020Lim[] = {1.1644f, 1.1644f, 1.1644f,
                                      0.0f, -0.1874f, 2.1418f,
                                      1.6781f, -0.6505f, 0.0f};
    static const float bt2020Full[] = {
        1.0f, 1.0f, 1.0f, 0.0f, -0.1646f, 1.8814f, 1.4746f, -0.5714f, 0.0f};

    switch (color_space) {
    case AVCOL_SPC_SMPTE170M:
    case AVCOL_SPC_BT470BG:
        return color_full ? bt601Full : bt601Lim;
    case AVCOL_SPC_BT709:
        return color_full ? bt709Full : bt709Lim;
    case AVCOL_SPC_BT2020_NCL:
    case AVCOL_SPC_BT2020_CL:
        return color_full ? bt2020Full : bt2020Lim;
    default:
        return bt601Lim;
    }
}

EglVideoRenderer::~EglVideoRenderer() {
}

bool EglVideoRenderer::initialize(DECODER_PARAMETERS *params) {
    m_width = params->width;
    m_height = params->height;
    if (0 < m_width) {
        m_widthPercent = FIFTY_PERCENT * m_height / m_width;
    }

    if (params->context == nullptr) {
        eglLog(LOG_INFO, "EglContextInit execute");
        return false;
    }
    OHNativeWindow *window = static_cast<OHNativeWindow *>(params->context);
    if ((nullptr == window) || (0 >= params->width) || (0 >= params->height)) {
        eglLog(LOG_ERROR, "EglContextInit: param error");
        return false;
    }
    m_eglWindow = static_cast<EGLNativeWindowType>(window);
    if (nullptr == m_eglWindow) {
        eglLog(LOG_ERROR, "m_eglWindow is null");
        return false;
    }
    // Init display.
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (EGL_TRUE != eglInitialize(m_eglDisplay, 0, 0)) {
        eglLog(LOG_ERROR, "m_eglWindow failed");
        return false;
    }
    EGLConfig eglConfig;
    EGLint numConfigs;
    EGLint configSpec[] = {
        // Key,value.
       EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_RED_SIZE, EGL_RED_SIZE_DEFAULT, EGL_GREEN_SIZE,
       EGL_GREEN_SIZE_DEFAULT, EGL_BLUE_SIZE, EGL_BLUE_SIZE_DEFAULT, EGL_ALPHA_SIZE,
       EGL_ALPHA_SIZE_DEFAULT, EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
       // End.
       EGL_NONE};
    const EGLint maxConfigSize = 1;
    
    if (!eglChooseConfig(m_eglDisplay, configSpec, &eglConfig, maxConfigSize, &numConfigs)) {
        eglLog(LOG_ERROR, "eglChooseConfig: unable to choose configs");
        return false;
    }


    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, eglConfig, m_eglWindow, nullptr);
    if (m_eglSurface == EGL_NO_SURFACE) {
        eglLog(LOG_ERROR, "eglCreateWindowSurface failed");
        return false;
    }

    const EGLint ctxAttr[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    m_eglContext = eglCreateContext(m_eglDisplay, eglConfig, EGL_NO_CONTEXT, ctxAttr);
    if (m_eglContext == EGL_NO_CONTEXT) {
        eglLog(LOG_ERROR, "eglCreateContext failed");
        return false;
    }

    if (EGL_TRUE != eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext)) {
        eglLog(LOG_ERROR, "eglMakeCurrent failed");
        return false;
    }
    Shader shader(vertexShaderWithMatrix, fragYUV420P);
    // Create program.
    m_program = shader.use();

    if (PROGRAM_ERROR == m_program) {
        eglLog(LOG_ERROR, "CreateProgram: unable to create program");
        return false;
    }
    static float ver[] = {
        1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f};
    GLuint apos = static_cast<GLuint>(glGetAttribLocation(m_program, "aPosition"));
    glEnableVertexAttribArray(apos);
    glVertexAttribPointer(apos, 3, GL_FLOAT, GL_FALSE, 0, ver);
    // 加入纹理坐标数据
    static float fragment[] = {
        1.0f, 0.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f};
    GLuint aTex = static_cast<GLuint>(glGetAttribLocation(m_program, "aTextCoord"));
    glEnableVertexAttribArray(aTex);
    glVertexAttribPointer(aTex, 2, GL_FLOAT, GL_FALSE, 0, fragment);
    int width = this->m_width;
    int height = this->m_height;
    glUniform1i(glGetUniformLocation(m_program, "yTexture"), 0);
    glUniform1i(glGetUniformLocation(m_program, "uTexture"), 1);
    glUniform1i(glGetUniformLocation(m_program, "vTexture"), 2);
    // 纹理ID
    m_texture_id[3] = {0};
    // 创建若干个纹理对象，并且得到纹理ID
    glGenTextures(3, m_texture_id);
    // 绑定纹理。后面的的设置和加载全部作用于当前绑定的纹理对象
    // GL_TEXTURE0、GL_TEXTURE1、GL_TEXTURE2 的就是纹理单元，GL_TEXTURE_1D、GL_TEXTURE_2D、CUBE_MAP为纹理目标
    // 通过 glBindTexture 函数将纹理目标和纹理绑定后，对纹理目标所进行的操作都反映到对纹理上
    glBindTexture(GL_TEXTURE_2D, m_texture_id[0]);
    // 缩小的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // 放大的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 设置纹理的格式和大小
    //  加载纹理到 OpenGL，读入 buffer 定义的位图数据，并把它复制到当前绑定的纹理对象
    //  当前绑定的纹理对象就会被附加上纹理图像。
    // width,height表示每几个像素公用一个yuv元素？比如width / 2表示横向每两个像素使用一个元素？
    glTexImage2D(GL_TEXTURE_2D,
                 0,                // 细节基本 默认0
                 GL_LUMINANCE,     // gpu内部格式 亮度，灰度图（这里就是只取一个亮度的颜色通道的意思）
                 width,            // 加载的纹理宽度。最好为2的次幂(这里对y分量数据当做指定尺寸算，但显示尺寸会拉伸到全屏？)
                 height,           // 加载的纹理高度。最好为2的次幂
                 0,                // 纹理边框
                 GL_LUMINANCE,     // 数据的像素格式 亮度，灰度图
                 GL_UNSIGNED_BYTE, // 像素点存储的数据类型
                 NULL              // 纹理的数据（先不传）
    );

    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, m_texture_id[1]);
    // 缩小的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D,
                 0,            // 细节基本 默认0
                 GL_LUMINANCE, // gpu内部格式 亮度，灰度图（这里就是只取一个颜色通道的意思）
                 width / 2,    // u数据数量为屏幕的4分之1
                 height / 2,
                 0,                // 边框
                 GL_LUMINANCE,     // 数据的像素格式 亮度，灰度图
                 GL_UNSIGNED_BYTE, // 像素点存储的数据类型
                 NULL              // 纹理的数据（先不传）
    );

    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, m_texture_id[2]);
    // 缩小的过滤器
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 设置纹理的格式和大小
    glTexImage2D(GL_TEXTURE_2D,
                 0,            // 细节基本 默认0
                 GL_LUMINANCE, // gpu内部格式 亮度，灰度图（这里就是只取一个颜色通道的意思）
                 width / 2,
                 height / 2,       // v数据数量为屏幕的4分之1
                 0,                // 边框
                 GL_LUMINANCE,     // 数据的像素格式 亮度，灰度图
                 GL_UNSIGNED_BYTE, // 像素点存储的数据类型
                 NULL              // 纹理的数据（先不传）
    );
    return true;
}

void EglVideoRenderer::renderFrame(AVFrame *frame) {
    int width = m_width;
    int height = m_height;
    glActiveTexture(GL_TEXTURE0);
    // 绑定y对应的纹理
    glBindTexture(GL_TEXTURE_2D, m_texture_id[0]);
    // 替换纹理，比重新使用glTexImage2D性能高多
    glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0,          // 相对原来的纹理的offset
                    width, height, // 加载的纹理宽度、高度。最好为2的次幂
                    GL_LUMINANCE, GL_UNSIGNED_BYTE,
                    frame->data[0]);
    // 激活第二层纹理，绑定到创建的纹理
    glActiveTexture(GL_TEXTURE1);
    // 绑定u对应的纹理
    glBindTexture(GL_TEXTURE_2D, m_texture_id[1]);
    // 替换纹理，比重新使用glTexImage2D性能高
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE,
                    frame->data[1]);

    // 激活第三层纹理，绑定到创建的纹理
    glActiveTexture(GL_TEXTURE2);
    // 绑定v对应的纹理
    glBindTexture(GL_TEXTURE_2D, m_texture_id[2]);
    // 替换纹理，比重新使用glTexImage2D性能高
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                    GL_UNSIGNED_BYTE,
                    frame->data[2]);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // 窗口显示，交换双缓冲区
    eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

void EglVideoRenderer::bindTexture(int id) {
    float borderColorInternal[] = {borderColor[id], 0.0f, 0.0f, 1.0f};
    glBindTexture(GL_TEXTURE_2D, m_texture_id[id]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // ohos 没有 GL_TEXTURE_BORDER_COLOR 符号
    //     glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
    //                      borderColorInternal);
    textureWidth[id] = id > 0 ? m_frame_width / 2 : m_frame_width;
    textureHeight[id] = id > 0 ? m_frame_height / 2 : m_frame_height;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth[id], textureHeight[id],
                 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
    glUniform1i(m_texture_uniform[id], id);
}

void EglVideoRenderer::checkAndUpdateScale(AVFrame *frame) {
    if ((m_frame_width != frame->width) || (m_frame_height != frame->height)) {

        m_frame_width = frame->width;
        m_frame_height = frame->height;

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                     GL_STATIC_DRAW);

        int positionLocation =
            glGetAttribLocation(m_program, "a_position");
        glEnableVertexAttribArray(positionLocation);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

        for (int i = 0; i < 3; i++) {
            if (m_texture_id[i]) {
                glDeleteTextures(1, &m_texture_id[i]);
            }
        }
        glGenTextures(3, m_texture_id);
        for (int i = 0; i < 3; i++) {
            bindTexture(i);
        }

        bool colorFull = frame->color_range == AVCOL_RANGE_JPEG;

        glUniform3fv(m_offset_location, 1, gl_color_offset(colorFull));
        glUniformMatrix3fv(m_yuvmat_location, 1, GL_FALSE,
                           gl_color_matrix(frame->colorspace, colorFull));

        float frameAspect = ((float)m_frame_height / (float)m_frame_width);
        float screenAspect = ((float)m_height / (float)m_width);

        if (frameAspect > screenAspect) {
            float multiplier = frameAspect / screenAspect;
            glUniform4f(m_uv_data_location, 0.5f - 0.5f * (1.0f / multiplier),
                        0.0f, multiplier, 1.0f);
        } else {
            float multiplier = screenAspect / frameAspect;
            glUniform4f(m_uv_data_location, 0.0f,
                        0.5f - 0.5f * (1.0f / multiplier), 1.0f, multiplier);
        }
    }
}
