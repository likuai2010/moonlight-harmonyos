/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <napi/native_api.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <EGL/eglplatform.h>

#ifndef XComponent_common_H
#define XComponent_common_H

/**
 * Vertex shader.
 */
const char VERTEX_SHADER[] = "#version 300 es\n"
    "layout(location = 0) in vec4 a_position;\n"
    "layout(location = 1) in vec4 a_color;   \n"
    "out vec4 v_color;                       \n"
    "void main()                             \n"
    "{                                       \n"
    "   gl_Position = a_position;            \n"
    "   v_color = a_color;                   \n"
    "}                                       \n";

/**
 * Vertex shader.
 */
const char VERTEX_SHADER_TEX[] = "#version 300 es\n"
    "layout(location = 0) in vec4 a_position;\n"
    "layout(location = 1) in vec2 a_texCoord;  \n"
    "out vec2 vTextCoord;                       \n"
    "void main()                             \n"
    "{                                       \n"
    "   gl_Position = vec4(a_position,1.0,1.0);            \n"
    "   vTextCoord = vec2((position.x * 0.5 + 0.5), (0.5 - position.y * 0.5));                \n"
    "}                                       \n";
const char FRAGMENT_SHADER_TEX[] = "#version 300 es\n"
    "precision mediump float;                  \n"
    "uniform sampler2D yTexture;              \n"
    "uniform sampler2D uTexture;            \n"
    "uniform sampler2D vTexture;            \n"
    "in vec2 vTextCoord;            \n"
    "out vec4 FragColor;            \n"
    "void main()                               \n"
    "{                                         \n"
    "vec3 yuv;     \n"
    "vec3 rgb;     \n"
    "yuv.x = texture(yTexture, vTextCoord).r;    \n"
    "yuv.y = texture(uTexture, vTextCoord).g - 0.5;   \n"
    "yuv.z = texture(vTexture, vTextCoord).b - 0.5;    \n"
    "rgb = mat3( 1.0, 1.0, 1.0, 0.0, -0.183, 1.816, 1.540, -0.459, 0.0 ) * yuv;    \n"
    "FragColor = vec4(rgb, 1.0);    \n"
    "}                                         \n";


/**
 * Fragment shader.
 */
const char FRAGMENT_SHADER[] = "#version 300 es\n"
    "precision mediump float;                  \n"
    "in vec4 v_color;                          \n"
    "out vec4 fragColor;                       \n"
    "void main()                               \n"
    "{                                         \n"
    "   fragColor = v_color;                   \n"
    "}                                         \n";




/**
 * Background color #182431.
 */
const GLfloat BACKGROUND_COLOR[] = { 24.0f / 255, 36.0f / 255, 49.0f / 255, 1.0f };

/**
 * Draw color #7E8FFB.
 */
const GLfloat DRAW_COLOR[] = { 126.0f / 255, 143.0f / 255, 251.0f / 255, 1.0f };

/**
 * Change color #92D6CC.
 */
const GLfloat CHANGE_COLOR[] = { 146.0f / 255, 214.0f / 255, 204.0f / 255, 1.0f };

/**
 * Background area.
 */
const GLfloat BACKGROUND_RECTANGLE_VERTICES[] = {
    -1.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, -1.0f,
    -1.0f, -1.0f
};

/**
 * Log print domain.
 */
const unsigned int LOG_PRINT_DOMAIN = 0xFF00;

/**
 * Get context parameter count.
 */
const size_t GET_CONTEXT_PARAM_CNT = 1;

/**
 * Fifty percent.
 */
const float FIFTY_PERCENT = 0.5;

/**
 * Pointer size.
 */
const GLint POINTER_SIZE = 2;

/**
 * Triangle fan size.
 */
const GLsizei TRIANGLE_FAN_SIZE = 4;

/**
 * Egl red size default.
 */
const int EGL_RED_SIZE_DEFAULT = 8;

/**
 * Egl green size default.
 */
const int EGL_GREEN_SIZE_DEFAULT = 8;

/**
 * Egl blue size default.
 */
const int EGL_BLUE_SIZE_DEFAULT = 8;

/**
 * Egl alpha size default.
 */
const int EGL_ALPHA_SIZE_DEFAULT = 8;

/**
 * Default x position.
 */
const int DEFAULT_X_POSITION = 0;

/**
 * Default y position.
 */
const int DEFAULT_Y_POSITION = 0;

/**
 * Gl red default.
 */
const GLfloat GL_RED_DEFAULT = 0.0;

/**
 * Gl green default.
 */
const GLfloat GL_GREEN_DEFAULT = 0.0;

/**
 * Gl blue default.
 */
const GLfloat GL_BLUE_DEFAULT = 0.0;

/**
 * Gl alpha default.
 */
const GLfloat GL_ALPHA_DEFAULT = 1.0;

/**
 * Program error.
 */
const GLuint PROGRAM_ERROR = 0;

/**
 * Rectangle vertices size.
 */
const int RECTANGLE_VERTICES_SIZE = 8;

/**
 * Position handle name.
 */
const char POSITION_NAME[] = "a_position";

/**
 * Position error.
 */
const GLint POSITION_ERROR = -1;

/**
 * Context type.
 */
enum ContextType {
    APP_LIFECYCLE,
    PAGE_LIFECYCLE,
};

/**
 * Config attribute list.
 */
const EGLint ATTRIB_LIST[] = {
    // Key,value.
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_RED_SIZE, EGL_RED_SIZE_DEFAULT,
    EGL_GREEN_SIZE, EGL_GREEN_SIZE_DEFAULT,
    EGL_BLUE_SIZE, EGL_BLUE_SIZE_DEFAULT,
    EGL_ALPHA_SIZE, EGL_ALPHA_SIZE_DEFAULT,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
    // End.
    EGL_NONE
};

/**
 * Context attributes.
 */
const EGLint CONTEXT_ATTRIBS[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
};
#endif // XComponent_common_H