

#ifndef MANCHESTERUNITEDPLAYER_FRAGMENTSHADER_H
#define MANCHESTERUNITEDPLAYER_FRAGMENTSHADER_H


#define GET_STR(x) #x
static const char *vertexSimpleTexture =
        "        #version 300 es\n"
        "        //输入的顶点坐标，会在程序指定将数据输入到该字段\n"
        "        layout (location = 0) in vec4 aPosition;\n"
        "        layout (location = 1) in vec2 aTexCoord;\n"

        "        out vec2 TexCoord;"
        "\n"
        "        void main() {\n"
        "            //直接把传入的坐标值作为传入渲染管线。gl_Position是OpenGL内置的\n"
        "            gl_Position = aPosition;\n"
        "            TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);\n"
        //                "            TexCoord = aTexCoord;\n"
        "        }";

//图元被光栅化为多少片段，就被调用多少次
static const char *fragSimpleTexture =
        "        #version 300 es\n"
        "        precision mediump float;\n"
        "        in vec2 TexCoord;\n"
        "        out vec4 FragColor;\n"
        "        //传入的纹理\n"
        "        uniform sampler2D ourTexture;\n"
        "        uniform sampler2D ourTexture1;\n"

        "        void main() {\n"
        "            //gl_FragColor是OpenGL内置的\n"
        //        "            FragColor = texture(ourTexture, TexCoord);\n"
        "            FragColor = mix(texture(ourTexture, TexCoord), texture(ourTexture1, TexCoord), 0.5);\n"
        "        }";

//图元被光栅化为多少片段，就被调用多少次
static const char *frag3DTexture =
        "        #version 300 es\n"
        "        precision mediump float;\n"
        "        in vec2 TexCoord;\n"
        "        out vec4 FragColor;\n"
        "        //传入的纹理\n"
        "        uniform sampler2D ourTexture;\n"
//        "        uniform sampler2D ourTexture1;\n"

        "        void main() {\n"
        "            //gl_FragColor是OpenGL内置的\n"
        "            FragColor = texture(ourTexture, TexCoord);\n"
//        "            FragColor = mix(texture(ourTexture, TexCoord), texture(ourTexture1, TexCoord), 0.5);\n"
        "        }";

//图元被光栅化为多少片段，就被调用多少次
static const char *frag3DGradientColor =
        "        #version 300 es\n"
        "        precision mediump float;\n"
        "        in vec4 vTextColor;//输入的颜色\n"
        "        out vec4 FragColor;\n"
        "        //传入的纹理\n"
        "        void main() {\n"
        "            //gl_FragColor是OpenGL内置的\n"
        "            FragColor = vTextColor;\n"
        "        }";

#define GET_STR(x) #x
static const char *vertexSimpleShape =
        "        #version 300 es\n"
        "        layout (location = 0) \n"
        "        in vec4 aPosition;//输入的顶点坐标，会在程序指定将数据输入到该字段\n"
        "\n"
        "        out\n"
        "        vec4 vTextColor;//输出的颜色\n"

        "        out\n"
        "        vec4 vPosition;//输出的坐标\n"
        "\n"
        "        void main() {\n"
        "            //直接把传入的坐标值作为传入渲染管线。gl_Position是OpenGL内置的\n"
        "            gl_Position = aPosition;\n"
        "            vPosition = aPosition;\n"
        "            gl_PointSize = 50.0;\n"
        //        "            vTextColor = vec4(aPosition.x ,aPosition.y ,aPosition.z,1.0);\n"
        "        }";

#define GET_STR(x) #x
static const char *vertexSimpleShapeWithColor =
        "        #version 300 es\n"
        "        layout (location = 0) \n"
        "        in vec4 aPosition;//输入的顶点坐标，会在程序指定将数据输入到该字段\n"//如果传入的向量是不够4维的，自动将前三个分量设置为0.0，最后一个分量设置为1.0

        "        layout (location = 1) \n"
        "        in vec4 aColor;//输入的顶点的颜色\n" //如果传入的向量是不够4维的，自动将前三个分量设置为0.0，最后一个分量设置为1.0
        "\n"
        "        out\n"
        "        vec4 vTextColor;//输出的颜色\n"
        "\n"
        "        void main() {\n"
        "            //直接把传入的坐标值作为传入渲染管线。gl_Position是OpenGL内置的\n"
        "            gl_Position = aPosition;\n"
        "            vTextColor = aColor;\n"
        "        }";

//顶点着色器，每个顶点执行一次，可以并行执行
#define GET_STR(x) #x
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
        "            gl_Position = uMatrix * aPosition;;\n"
        "        }";


//支持3D效果的顶点着色器，每个顶点执行一次，可以并行执行
#define GET_STR(x) #x
static const char *vertexShader3D =
        "        #version 300 es\n"
        "        layout (location = 0) in vec4 aPosition;//输入的顶点坐标，会在程序指定将数据输入到该字段\n"//如果传入的向量是不够4维的，自动将前三个分量设置为0.0，最后一个分量设置为1.0

        "        layout (location = 1) in vec2 aTextCoord;//输入的纹理坐标，会在程序指定将数据输入到该字段\n"
        "\n"
        "        out vec2 TexCoord;//输出的纹理坐标;\n"
        "        uniform mat4 model;\n"
        "        uniform mat4 view;\n"
        "        uniform mat4 projection;\n"
        "\n"
        "        void main() {\n"
        "            //这里其实是将上下翻转过来（因为安卓图片会自动上下翻转，所以转回来）\n"
        "             TexCoord = vec2(aTextCoord.x, 1.0 - aTextCoord.y);\n"
        "            //直接把传入的坐标值作为传入渲染管线。gl_Position是OpenGL内置的\n"
        "         gl_Position = projection * view * model * aPosition;\n"
        "        }";


//支持3D效果的顶点着色器，每个顶点执行一次，可以并行执行
#define GET_STR(x) #x
static const char *vertexShader3DGradientColor =
        "        #version 300 es\n"
        "        layout (location = 0) in vec4 aPosition;//输入的顶点坐标，会在程序指定将数据输入到该字段\n"//如果传入的向量是不够4维的，自动将前三个分量设置为0.0，最后一个分量设置为1.0
        "        layout (location = 1) in vec4 aColor;//输入的颜色，会在程序指定将数据输入到该字段\n"

        "        out vec4 vTextColor;//输出的颜色\n"
        "\n"
        "        out vec2 TexCoord;//输出的纹理坐标;\n"
        "        uniform mat4 model;\n"
        "        uniform mat4 view;\n"
        "        uniform mat4 projection;\n"
        "\n"
        "        void main() {\n"
        "            //直接把传入的坐标值作为传入渲染管线。gl_Position是OpenGL内置的\n"
        "            gl_Position = projection * view * model * aPosition;\n"
        "            vTextColor = aColor;\n"
        "        }";

//图元被光栅化为多少片段，就被调用多少次
static const char *fragSimpleShape =
        "  #version 300 es\n"
        "        precision\n"
        "        mediump float;\n"
        "\n"
        "        in\n"
        "        vec4 vTextColor;//输入的颜色\n"
        "        out vec4 FragColor;\n"

        "        in\n"
        "        vec4 vPosition;//输入的坐标\n"

        "        void main() {\n"
        "            //gl_FragColor是OpenGL内置的\n"
        "            FragColor = vec4(vPosition.x ,vPosition.y ,vPosition.z,1.0);\n"
        "        }";

//图元被光栅化为多少片段，就被调用多少次
static const char *fragSimpleShapeWithColor =
        "  #version 300 es\n"
        "        precision\n"
        "        mediump float;\n"
        "\n"
        "        in\n"
        "        vec4 vTextColor;//输入的颜色\n"
        "        out vec4 FragColor;\n"

        "        in\n"
        "        vec4 vPosition;//输入的坐标\n"

        "        void main() {\n"
        "            //gl_FragColor是OpenGL内置的\n"
        "            FragColor = vTextColor;\n"
        "        }";

//图元被光栅化为多少片段，就被调用多少次
static const char *fragSimpleShapeEBO =
        "  #version 300 es\n"
        "        precision\n"
        "        mediump float;\n"
        "\n"
        "        in\n"
        "        vec4 vTextColor;//输入的颜色\n"
        "        out vec4 FragColor;\n"

        "        in\n"
        "        vec4 vPosition;//输入的坐标\n"

        "        void main() {\n"
        "            FragColor = vTextColor;\n"
        //        "            FragColor = vec4(1.0 ,0.0 ,0.0 ,1.0);\n"
        "        }";

static const char *fragSimpleUniform =
        "  #version 300 es\n"
        "        precision\n"
        "        mediump float;\n"
        "\n"
        "        uniform\n"
        "        vec4 uTextColor;//输出的颜色\n"
        "out vec4 FragColor;\n"
        "\n"
        "        void main() {\n"
        "            FragColor = uTextColor;\n"
        "        }";

static const char *vertexSimpleUniform =
        "        #version 300 es\n"
        "        layout (location = 0) \n"
        "        in vec4 aPosition;//输入的顶点坐标，会在程序指定将数据输入到该字段\n"
        "\n"
        //        "        uniform\n"
        //        "        vec4 vTextColor;//输出的颜色\n"
        "\n"
        "        void main() {\n"
        "            //直接把传入的坐标值作为传入渲染管线。gl_Position是OpenGL内置的\n"
        "            gl_Position = aPosition;\n"
        "        }";

//图元被光栅化为多少片段，就被调用多少次
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

static const char *fragYUV420PDivideTo2 =
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

        "    vec2 uv = vTextCoord.xy;\n"
        "    float y;\n"
        "    if (uv.y >= 0.0 && uv.y <= 0.5) {\n"
        "        uv.y = uv.y + 0.25;\n"
        "    }else{\n"
        "        uv.y = uv.y - 0.25;\n"
        "    }\n"
        "    //分别取yuv各个分量的采样纹理\n"
        "    yuv.x = texture(yTexture, uv).r;\n"
        "   yuv.y = texture(uTexture, uv).g - 0.5;\n"
        "    yuv.z = texture(vTexture, uv).b - 0.5;\n"
        "   rgb = mat3(\n"
        "            1.0, 1.0, 1.0,\n"
        "            0.0, -0.183, 1.816,\n"
        "            1.540, -0.459, 0.0\n"
        "    ) * yuv;\n"
        "    //gl_FragColor是OpenGL内置的\n"
        "    FragColor = vec4(rgb, 1.0);\n"
        " }";


static const char *fragYUV420PDivideTo4 =
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

        "    vec2 uv = vTextCoord.xy;\n"
        "    if (uv.x <= 0.5) {\n"
        "        uv.x = uv.x * 2.0;\n"
        "    }else{\n"
        "        uv.x = (uv.x - 0.5) * 2.0;\n"
        "    }\n"
        "    \n"
        "     if (uv.y <= 0.5) {\n"
        "           uv.y = uv.y * 2.0;\n"
        "       }else{\n"
        "           uv.y = (uv.y - 0.5) * 2.0;\n"
        "       }\n"
        "    //分别取yuv各个分量的采样纹理\n"
        "    yuv.x = texture(yTexture, uv).r;\n"
        "   yuv.y = texture(uTexture, uv).g - 0.5;\n"
        "    yuv.z = texture(vTexture, uv).b - 0.5;\n"
        "   rgb = mat3(\n"
        "            1.0, 1.0, 1.0,\n"
        "            0.0, -0.183, 1.816,\n"
        "            1.540, -0.459, 0.0\n"
        "    ) * yuv;\n"
        "    //gl_FragColor是OpenGL内置的\n"
        "    FragColor = vec4(rgb, 1.0);\n"
        " }";

/**
 * 灵魂出鞘
 */
static const char *fragSoulFled =
        "#version 300 es\n"

        "precision mediump float;\n"
        "//纹理坐标\n"
        "in vec2 vTextCoord;\n"
        "uniform float uScale;\n"
        "//输入的yuv三个纹理\n"
        "uniform sampler2D yTexture;//采样器\n"
        "uniform sampler2D uTexture;//采样器\n"
        "uniform sampler2D vTexture;//采样器\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "vec3 frontYuv;\n"
        "vec3 rgb;\n"
        "vec3 frontRgb;\n"
        "vec2 frontTextCoord = vTextCoord.xy;"
        "//采样到的yuv向量数据\n"
        "vec3 yuv;\n"
        "//yuv转化得到的rgb向量数据\n"
        "//分别取yuv各个分量的采样纹理\n"
        "yuv.x = texture(yTexture, vTextCoord).r;\n"
        "yuv.y = texture(uTexture, vTextCoord).g - 0.5;\n"
        "yuv.z = texture(vTexture, vTextCoord).b - 0.5;\n"
        "rgb = mat3(\n"
        "            1.0, 1.0, 1.0,\n"
        "            0.0, -0.183, 1.816,\n"
        "            1.540, -0.459, 0.0\n"
        "    ) * yuv;\n"
        "//将纹理坐标中心转成(0.0, 0.0)再做缩放\n"
        "vec2 center = vec2(0.5, 0.5);\n"
        "frontTextCoord = frontTextCoord - center;\n"
        "frontTextCoord = frontTextCoord / uScale;\n"
        "frontTextCoord = frontTextCoord + center;\n"
        "//对缩放后的坐标位置进行采样\n"
        "frontYuv.r = texture(yTexture, frontTextCoord).r;\n"
        "frontYuv.g = texture(uTexture, frontTextCoord).r - 0.5;\n"
        "//NV12会把V采样到a通道\n"
        "frontYuv.b = texture(vTexture, frontTextCoord).r - 0.5;"
        "frontRgb = mat3(\n"
        "       1.0, 1.0, 1.0,\n"
        "       0.0, -0.183, 1.816,\n"
        "       1.540, -0.459, 0.0\n"
        "        ) * frontYuv;\n"
        "// 原坐标的片段颜色和缩放后坐标片段颜色进行线性混合\n"
        " FragColor = mix(vec4(rgb, 1.0), vec4(frontRgb, 1.0), 1.0 - fract(uScale));\n"
        "   // FragColor = vec4(rgb, 1.0);\n"
        "}";

/**
 * 可缩放的顶点着色器
 */
#define GET_STR(x) #x
static const char *scaleVertexShader =
        "#version 300 es\n"
        "uniform float u_time;"
        "varying float time;"
        "attribute"
        "vec4 aPosition;//输入的顶点坐标"
        "attribute"
        "vec2 aTextCoord;//输入的纹理坐标"
        "varying"
        "vec2 vTextCoord;//输出的纹理坐标"
        //缩放矩阵
        "uniform"
        "mat4 uMatrix;"
        "void main() {"
        "   //这里其实是将上下翻转过来（因为安卓图片会自动上下翻转，所以转回来）"
        "    vTextCoord = vec2(aTextCoord.x, 1.0 - aTextCoord.y);"
        "    gl_Position = uMatrix * aPosition;"
        "    time = u_time;"
        " }";
/**
 * 普通yuv420p shader
 */
//static const char *fragYUV420P = GET_STR(
//        precision
//        mediump float;
//        varying
//        vec2 vTextCoord;
//        //输入的yuv三个纹理
//        uniform
//        sampler2D yTexture;
//        uniform
//        sampler2D uTexture;
//        uniform
//        sampler2D vTexture;
//        void main() {
//            vec3 yuv;
//            vec3 rgb;
//            //分别取yuv各个分量的采样纹理（r表示？）
//            yuv.r = texture2D(yTexture, vTextCoord).r;
//            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
//            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
//            rgb = mat3(
//                    1.0, 1.0, 1.0,
//                    0.0, -0.39465, 2.03211,
//                    1.13983, -0.5806, 0.0
//            ) * yuv;
//            gl_FragColor = vec4(rgb, 1.0);
//        }
//);
/**
 * yuv420p反色 shader
 */
static const char *fragYUV420POppositeColor =
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
        "    //分别取yuv各个分量的采样纹理（r表示？）\n"
        "    yuv.x = texture(yTexture, vTextCoord).r;\n"
        "    yuv.y = texture(uTexture, vTextCoord).g - 0.5;\n"
        "    yuv.z = texture(vTexture, vTextCoord).b - 0.5;\n"
        "   rgb = mat3(\n"
        "            1.0, 1.0, 1.0,\n"
        "            0.0, -0.183, 1.816,\n"
        "            1.540, -0.459, 0.0\n"
        "    ) * yuv;\n"
        "    //gl_FragColor是OpenGL内置的\n"
        "    FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);\n"
        " }";

/**
 * yuv420p灰度shader
 */
static const char *fragYUV420PGray =
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
        "    //分别取yuv各个分量的采样纹理（r表示？）\n"
        "    yuv.x = texture(yTexture, vTextCoord).r;\n"
        "   yuv.y = 0.0;\n"
        "    yuv.z = 0.0;\n"
        "   rgb = mat3(\n"
        "            1.0, 1.0, 1.0,\n"
        "            0.0, -0.183, 1.816,\n"
        "            1.540, -0.459, 0.0\n"
        "    ) * yuv;\n"
        "    //gl_FragColor是OpenGL内置的\n"
        //        "     float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;\n"
        "     FragColor = vec4(rgb, 1.0);\n"
        " }";

/**
 * yuv420p使用反色和灰度图轮播效果滤镜 shader
 */
static const char *fragYUV420POppoColorAndGray =
        "#version 300 es\n"

        "precision mediump float;\n"
        "//纹理坐标\n"
        "in vec2 vTextCoord;\n"
        "//输入的yuv三个纹理\n"
        "uniform sampler2D yTexture;//采样器\n"
        "uniform sampler2D uTexture;//采样器\n"
        "uniform sampler2D vTexture;//采样器\n"
        "out vec4 FragColor;\n"
        // "uniform float time;"
        "void main() {\n"
        "//采样到的yuv向量数据\n"
        "   vec3 yuv;\n"
        "//yuv转化得到的rgb向量数据\n"
        "    vec3 rgb;\n"
        "    //分别取yuv各个分量的采样纹理（r表示？）\n"
        "    yuv.x = texture(yTexture, vTextCoord).r;\n"
        "   yuv.y = texture(uTexture, vTextCoord).g - 0.5;\n"
        "    yuv.z = texture(vTexture, vTextCoord).b - 0.5;\n"
        "   rgb = mat3(\n"
        "            1.0, 1.0, 1.0,\n"
        "            0.0, -0.183, 1.816,\n"
        "            1.540, -0.459, 0.0\n"
        "    ) * yuv;\n"
        "    //gl_FragColor是OpenGL内置的\n"
        //   "     float filterType = sin(time / 400.0);\n"
        //   "            if (filterType > 0.0) {\n"
        "                if (vTextCoord.x < 0.5 && vTextCoord.y < 0.5) {\n"
        "                    //反色滤镜\n"
        "                    FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);\n"
        "                } else if (vTextCoord.x > 0.5 && vTextCoord.y > 0.5) {\n"
        "                    float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;\n"
        "                    FragColor = vec4(gray, gray, gray, 1.0);\n"
        "                } else {\n"
        "                    FragColor = vec4(rgb, 1.0);\n"
        "\n"
        "                }\n"
        //        "            } else {\n"
        //        "                if (vTextCoord.x > 0.5 && vTextCoord.y < 0.5) {\n"
        //        "                    //反色滤镜\n"
        //        "                    gl_FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);\n"
        //        "                } else if (vTextCoord.x < 0.5 && vTextCoord.y > 0.5) {\n"
        //        "                    float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;\n"
        //        "                    gl_FragColor = vec4(gray, gray, gray, 1.0);\n"
        //        "                } else {\n"
        //        "                    gl_FragColor = vec4(rgb, 1.0);\n"
        //        "\n"
        //        "                }\n"
        //    "            }\n"
        " }";

//static const char *GAUSSIAN_BLUR_VERTEX_SHADER =
//        "attribute vec4 position;\n"
//        "attribute vec4 inputTextureCoordinate;\n"
//        "\n"
//        "const int GAUSSIAN_SAMPLES = 9;\n"
//        "\n"
//        "uniform float texelWidthOffset;\n"
//        "uniform float texelHeightOffset;\n"
//        "\n"
//        "varying vec2 textureCoordinate;\n"
//        "varying vec2 blurCoordinates[GAUSSIAN_SAMPLES];\n"
//        "\n"
//        "void main()\n"
//        "{\n"
//        "	gl_Position = position;\n"
//        "	textureCoordinate = inputTextureCoordinate.xy;\n"
//        "	\n"
//        "	// Calculate the positions for the blur\n"
//        "	int multiplier = 0;\n"
//        "	vec2 blurStep;\n"
//        "   vec2 singleStepOffset = vec2(texelHeightOffset, texelWidthOffset);\n"
//        "    \n"
//        "	for (int i = 0; i < GAUSSIAN_SAMPLES; i++)\n"
//        "   {\n"
//        "		multiplier = (i - ((GAUSSIAN_SAMPLES - 1) / 2));\n"
//        "       // Blur in x (horizontal)\n"
//        "       blurStep = float(multiplier) * singleStepOffset;\n"
//        "		blurCoordinates[i] = inputTextureCoordinate.xy + blurStep;\n"
//        "	}\n"
//        "}\n";

static const char *GAUSSIAN_BLUR_VERTEX_SHADER =
        "        #version 300 es\n"
        "        layout (location = 0) \n"
        "        in vec4 aPosition;//输入的顶点坐标，会在程序指定将数据输入到该字段\n"//如果传入的向量是不够4维的，自动将前三个分量设置为0.0，最后一个分量设置为1.0

        "        layout (location = 1) \n"
        "        in vec2 aTextCoord;//输入的纹理坐标，会在程序指定将数据输入到该字段\n"
        "\n"
        "        out vec2 vTextCoord;//输出的纹理坐标;\n"
        "        uniform mat4 uMatrix;"//变换矩阵
        "        const int GAUSSIAN_SAMPLES = 9;\n"
        "        out vec2 blurCoordinates[GAUSSIAN_SAMPLES];\n"
        "\n"
        "        void main() {\n"
        "            //这里其实是将上下翻转过来（因为安卓图片会自动上下翻转，所以转回来）\n"
        "             vTextCoord = vec2(aTextCoord.x, 1.0 - aTextCoord.y);\n"
        "            //直接把传入的坐标值作为传入渲染管线。gl_Position是OpenGL内置的\n"
        //    "            gl_Position = aPosition;\n"
        "            gl_Position = uMatrix * aPosition;\n"
        //        "            vec2 widthStep = vec2(offset*widthFactor, 0.0);\n"
        "            vec2 widthStep = vec2(10.0/1080.0, 0.0);\n"
        //        "            vec2 heightStep = vec2(0.0, offset*heightFactor);\n"
        "            vec2 heightStep = vec2(0.0, 10.0/1920.0);\n"
        "            blurCoordinates[0] = vTextCoord.xy - heightStep - widthStep; // 左上\n"
        "            blurCoordinates[1] = vTextCoord.xy - heightStep; // 上\n"
        "            blurCoordinates[2] = vTextCoord.xy - heightStep + widthStep; // 右上\n"
        "            blurCoordinates[3] = vTextCoord.xy - widthStep; // 左中\n"
        "            blurCoordinates[4] = vTextCoord.xy; // 中\n"
        "            blurCoordinates[5] = vTextCoord.xy + widthStep; // 右中\n"
        "            blurCoordinates[6] = vTextCoord.xy + heightStep - widthStep; // 左下\n"
        "            blurCoordinates[7] = vTextCoord.xy + heightStep; // 下\n"
        "            blurCoordinates[8] = vTextCoord.xy + heightStep + widthStep; // 右下\n"
        "        }";

//static const char *GAUSSIAN_BLUR_FRAGMENT_SHADER =
//        "uniform sampler2D inputImageTexture;\n"
//        "\n"
//        "const lowp int GAUSSIAN_SAMPLES = 9;\n"
//        "\n"
//        "varying highp vec2 textureCoordinate;\n"
//        "varying highp vec2 blurCoordinates[GAUSSIAN_SAMPLES];\n"
//        "\n"
//        "void main()\n"
//        "{\n"
//        "	lowp vec3 sum = vec3(0.0);\n"
//        "   lowp vec4 fragColor=texture2D(inputImageTexture,textureCoordinate);\n"
//        "	\n"
//        "    sum += texture2D(inputImageTexture, blurCoordinates[0]).rgb * 0.05;\n"
//        "    sum += texture2D(inputImageTexture, blurCoordinates[1]).rgb * 0.09;\n"
//        "    sum += texture2D(inputImageTexture, blurCoordinates[2]).rgb * 0.12;\n"
//        "    sum += texture2D(inputImageTexture, blurCoordinates[3]).rgb * 0.15;\n"
//        "    sum += texture2D(inputImageTexture, blurCoordinates[4]).rgb * 0.18;\n"
//        "    sum += texture2D(inputImageTexture, blurCoordinates[5]).rgb * 0.15;\n"
//        "    sum += texture2D(inputImageTexture, blurCoordinates[6]).rgb * 0.12;\n"
//        "    sum += texture2D(inputImageTexture, blurCoordinates[7]).rgb * 0.09;\n"
//        "    sum += texture2D(inputImageTexture, blurCoordinates[8]).rgb * 0.05;\n"
//        "\n"
//        "	gl_FragColor = vec4(sum,fragColor.a);\n"
//        "}";


static const char *GAUSSIAN_BLUR_FRAGMENT_SHADER =
        "#version 300 es\n"

        "precision mediump float;\n"
        "//纹理坐标\n"
        "in vec2 vTextCoord;\n"
        "//输入的yuv三个纹理\n"
        "uniform sampler2D yTexture;//采样器\n"
        "uniform sampler2D uTexture;//采样器\n"
        "uniform sampler2D vTexture;//采样器\n"
        "out vec4 FragColor;\n"
        "const lowp int GAUSSIAN_SAMPLES = 9;\n"
        "in highp vec2 blurCoordinates[GAUSSIAN_SAMPLES];\n"

        "mat3 kernelMatrix = mat3(\n"
        "0.0947416f, 0.118318f, 0.0947416f,\n"
        "                0.118318f,  0.147761f, 0.118318f,\n"
        "                0.0947416f, 0.118318f, 0.0947416f\n"
        ");\n"

        "mat3 colorConversionMatrix = mat3(\n"
        "                   1.0, 1.0, 1.0,\n"
        "                   0.0, -0.39465, 2.03211,\n"
        "                   1.13983, -0.58060, 0.0"
        ");\n"
        "\n"
        "vec3 yuv2rgb(vec2 pos)\n"
        "{\n"
        "       vec3 yuv;"
        "       yuv.x = texture(yTexture, pos).r;\n"
        "       yuv.y = texture(uTexture, pos).r - 0.5;\n"
        "       yuv.z = texture(vTexture, pos).r - 0.5;\n"
        "       return colorConversionMatrix * yuv;\n"
        "}\n"

        "void main() {\n"
        "       //采样到的yuv向量数据\n"
        "   vec3 yuv;\n"
        "       //yuv转化得到的rgb向量数据\n"
        //        "lowp vec3 fragColor=yuv2rgb(vTextCoord);\n"
        //        "lowp vec3 sum = vec3(0.0);\n"
        "       lowp vec3 sum = (yuv2rgb(blurCoordinates[0]) * kernelMatrix[0][0]);\n"
        "       sum += (yuv2rgb(blurCoordinates[1]) * kernelMatrix[0][1]);\n"
        "       sum += (yuv2rgb(blurCoordinates[2]) * kernelMatrix[0][2]);\n"
        "       sum += (yuv2rgb(blurCoordinates[3]) * kernelMatrix[1][0]);\n"
        "       sum += (yuv2rgb(blurCoordinates[4]) * kernelMatrix[1][1]);\n"
        "       sum += (yuv2rgb(blurCoordinates[5]) * kernelMatrix[1][2]);\n"
        "       sum += (yuv2rgb(blurCoordinates[6]) * kernelMatrix[2][0]);\n"
        "       sum += (yuv2rgb(blurCoordinates[7]) * kernelMatrix[2][1]);\n"
        "       sum += (yuv2rgb(blurCoordinates[8]) * kernelMatrix[2][2]);\n"
        "       FragColor = vec4(sum,1.0);\n"
        "}";

//
//        GET_STR(
//        precision
//        mediump float;
//        varying
//        vec2 vTextCoord;
//        //输入的yuv三个纹理
//        uniform
//        sampler2D yTexture;
//        uniform
//        sampler2D uTexture;
//        uniform
//        sampler2D vTexture;
//        varying float time;
//        void main() {
//            vec3 yuv;
//            vec3 rgb;
//            //分别取yuv各个分量的采样纹理（r表示？）
//            yuv.r = texture2D(yTexture, vTextCoord).r;
//            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
//            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
//            rgb = mat3(
//                    1.0, 1.0, 1.0,
//                    0.0, -0.39465, 2.03211,
//                    1.13983, -0.5806, 0.0
//            ) * yuv;
//
//            float filterType = sin(time / 400.0);
//            if (filterType > 0.0) {
//                if (vTextCoord.x < 0.5 && vTextCoord.y < 0.5) {
//                    //反色滤镜
//                    gl_FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);
//                } else if (vTextCoord.x > 0.5 && vTextCoord.y > 0.5) {
//                    float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;
//                    gl_FragColor = vec4(gray, gray, gray, 1.0);
//                } else {
//                    gl_FragColor = vec4(rgb, 1.0);
//
//                }
//            } else {
//                if (vTextCoord.x > 0.5 && vTextCoord.y < 0.5) {
//                    //反色滤镜
//                    gl_FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);
//                } else if (vTextCoord.x < 0.5 && vTextCoord.y > 0.5) {
//                    float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;
//                    gl_FragColor = vec4(gray, gray, gray, 1.0);
//                } else {
//                    gl_FragColor = vec4(rgb, 1.0);
//
//                }
//            }
//
//
//        }
//);

/**
 * NV12 shader
 */
static const char *fragNV12 = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

/**
 * NV12反色 shader
 */
static const char *fragNV12OppoColor = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            gl_FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);
        }
);

/**
 * NV12灰度 shader
 */
static const char *fragNV12Gray = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;
            gl_FragColor = vec4(gray, gray, gray, 1.0);
        }
);
/**
 * NV21 shader
 */
static const char *fragNV21 = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);


/**
 * NV21反色 shader
 */
static const char *fragNV21OppoColor = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            gl_FragColor = vec4(vec3(1.0 - rgb.r, 1.0 - rgb.g, 1.0 - rgb.b), 1.0);
        }
);

/**
 * NV21灰度 shader
 */
static const char *fragNV21Gray = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            float gray = rgb.r * 0.2125 + rgb.g * 0.7154 + rgb.b * 0.0721;
            gl_FragColor = vec4(gray, gray, gray, 1.0);
        }
);

/**
 * YUV420P闪白
 */
static const char *fragYUV420PSplash = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        varying float time;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            //除以1.3是为了减小变化的幅度，使得不出现一段时间的全白色。减0.2是为了给原始颜色画面提供停留的时间
            //因为这里的time单位为毫秒，所以要除以一个比较大的数才可以保持一个肉眼可见的闪动效果
            float uAdditionalColor = abs(sin(time / 150.0)) - 0.3;
            if (uAdditionalColor < 0.0) {
                uAdditionalColor = 0.0;
            }
            gl_FragColor = vec4(rgb.r + uAdditionalColor, rgb.g + uAdditionalColor,
                                rgb.b + uAdditionalColor, 1.0);
        }
);

/**
 * NV21闪白
 */
static const char *fragNV21Splash = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        varying float time;

        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            //除以1.3是为了减小变化的幅度，使得不出现一段时间的全白色。减0.2是为了给原始颜色画面提供停留的时间
            //因为这里的time单位为毫秒，所以要除以一个比较大的数才可以保持一个肉眼可见的闪动效果
            float uAdditionalColor = abs(sin(time / 150.0)) - 0.3;
            if (uAdditionalColor < 0.0) {
                uAdditionalColor = 0.0;
            }

            gl_FragColor = vec4(rgb.r + uAdditionalColor, rgb.g + uAdditionalColor,
                                rgb.b + uAdditionalColor, 1.0);
        }
);


/**
 * NV12闪白
 */
static const char *fragNV12Splash = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        //修改这个值，可以控制曝光的程度
        // uniform float uAdditionalColor;
        varying float time;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            //除以1.3是为了减小变化的幅度，使得不出现一段时间的全白色。减0.2是为了给原始颜色画面提供停留的时间
            //因为这里的time单位为毫秒，所以要除以一个比较大的数才可以保持一个肉眼可见的闪动效果
            float uAdditionalColor = abs(sin(time / 150.0)) - 0.3;
            if (uAdditionalColor < 0.0) {
                uAdditionalColor = 0.0;
            }

            gl_FragColor = vec4(rgb.r + uAdditionalColor, rgb.g + uAdditionalColor,
                                rgb.b + uAdditionalColor, 1.0);
        }
);

/**
 * YUV420P缩放
 */
static const char *fragYUV420PScale = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        varying float time;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

/**
 * NV21缩放
 */
static const char *fragNV21Scale = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        varying float time;

        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            gl_FragColor = vec4(rgb, 1.0);
        }
);


/**
 * NV12缩放
 */
static const char *fragNV12Scale = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        //修改这个值，可以控制曝光的程度
        // uniform float uAdditionalColor;

        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            gl_FragColor = vec4(rgb, 1.0);
        }
);

/**
 * YUV420P灵魂出窍
 */
static const char *fragYUV420PSoul = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        varying float time;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

/**
 * NV21灵魂出窍
 */
static const char *fragNV21Soul = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        varying float time;

        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            gl_FragColor = vec4(rgb, 1.0);
        }
);


/**
 * NV12灵魂出窍
 */
static const char *fragNV12Soul = GET_STR(
        precision
        mediump float;
        varying
                vec2
        vTextCoord;
//输入的yuv三个纹理
        uniform
                sampler2D
        yTexture;
        uniform
                sampler2D
        uvTexture;
        uniform float uAlpha;
        uniform float uScale;

        void main() {
            vec3 yuv;
            vec3 frontYuv;
            vec3 rgb;
            vec3 frontRgb;
            vec2 frontTextCoord = vTextCoord.xy;
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            // 将纹理坐标中心转成(0.0, 0.0)再做缩放
            vec2 center = vec2(0.5, 0.5);
            frontTextCoord = frontTextCoord - center;
            frontTextCoord = frontTextCoord / uScale;
            frontTextCoord = frontTextCoord + center;

            frontYuv.r = texture2D(yTexture, frontTextCoord).r;
            frontYuv.g = texture2D(uvTexture, frontTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            frontYuv.b = texture2D(uvTexture, frontTextCoord).a - 0.5;
            frontRgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * frontYuv;

            //gl_FragColor = vec4(frontRgb, 1.0);

            // 线性混合
            gl_FragColor = mix(vec4(rgb, 1.0), vec4(frontRgb, 1.0), 1.0 - fract(uScale));
        }

);

/**
 * YUV420P抖动（todo 暂未完成）
 */
static const char *fragYUV420PShake = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uTexture;
        uniform
        sampler2D vTexture;
        varying float time;
        //颜色的偏移距离
        uniform float uTextureCoordOffset;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理（r表示？）
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uTexture, vTextCoord).r - 0.5;
            yuv.b = texture2D(vTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

/**
 * NV21抖动（todo 暂未完成）
 */
static const char *fragNV21Shake = GET_STR(
        precision
        mediump float;
        varying
        vec2 vTextCoord;
        //输入的yuv三个纹理
        uniform
        sampler2D yTexture;
        uniform
        sampler2D uvTexture;
        varying float time;
        //颜色的偏移距离
        uniform float uTextureCoordOffset;
        void main() {
            vec3 yuv;
            vec3 rgb;
            //分别取yuv各个分量的采样纹理
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).a - 0.5;
            yuv.b = texture2D(uvTexture, vTextCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            gl_FragColor = vec4(rgb, 1.0);
        }
);


/**
 * NV12抖动
 */
static const char *fragNV12Shake = GET_STR(
        precision
        mediump float;
        varying
                vec2
        vTextCoord;
        //输入的yuv三个纹理
        uniform
                sampler2D
        yTexture;
        uniform
                sampler2D
        uvTexture;
        //颜色的偏移距离
        uniform float uTextureCoordOffset;

        void main() {
            //当前片段
            vec3 yuv;
            //左上片段
            vec3 yuvLeftTop;
            //右下片段
            vec3 yuvRightBottom;

            //当前片段
            vec3 rgb;
            //左上片段
            vec3 rgbLeftTop;
            //右下片段
            vec3 rgbRightBottom;
            //设置了位置偏移采样到的颜色
            vec2 leftTopTexCoord = vec2(vTextCoord.x + uTextureCoordOffset,
                                        vTextCoord.y + uTextureCoordOffset);
            vec2 rightBottomTexCoord = vec2(vTextCoord.x - uTextureCoordOffset,
                                            vTextCoord.y - uTextureCoordOffset);
            //分别取yuv各个分量的采样纹理（r表示？）
            //这里texture2D(yTexture, vTextCoord).r取.g.b效果也是一样的
            yuv.r = texture2D(yTexture, vTextCoord).r;
            yuv.g = texture2D(uvTexture, vTextCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuv.b = texture2D(uvTexture, vTextCoord).a - 0.5;

            yuvLeftTop.r = texture2D(yTexture, leftTopTexCoord).r;
            yuvLeftTop.g = texture2D(uvTexture, leftTopTexCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuvLeftTop.b = texture2D(uvTexture, leftTopTexCoord).a - 0.5;


            yuvRightBottom.r = texture2D(yTexture, rightBottomTexCoord).r;
            yuvRightBottom.g = texture2D(uvTexture, rightBottomTexCoord).r - 0.5;
            //NV12会把V采样到a通道
            yuvRightBottom.b = texture2D(uvTexture, rightBottomTexCoord).a - 0.5;


            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;

            rgbLeftTop = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuvLeftTop;

            yuvRightBottom = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.39465, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuvRightBottom;

            gl_FragColor = vec4(rgbLeftTop.r, yuvRightBottom.g, rgb.b, 1.0);
        }

);


#endif //MANCHESTERUNITEDPLAYER_FRAGMENTSHADER_H
