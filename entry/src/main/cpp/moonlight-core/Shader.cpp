

#include "Shader.h"


GLint Shader::initShader(const char *source, GLint type) {
    //创建shader
    GLint sh = glCreateShader(type);
    if (sh == 0) {
        Shader_LOGD("glCreateShader %d failed", type);
        return 0;
    }
    //加载shader
    glShaderSource(sh,
                   1,//shader数量
                   &source,
                   0);//代码长度，传0则读到字符串结尾

    //编译shader
    glCompileShader(sh);

    //打印出编译错误信息
    GLint status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);
    if (status == 0) {
        Shader_LOGD("glCompileShader %d failed", type);
        Shader_LOGD("source %s", source);
        auto *infoLog = new GLchar[2048];
        GLsizei length;
        glGetShaderInfoLog(sh, 2048, &length, infoLog);
//        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;

        Shader_LOGD("ERROR::SHADER::VERTEX::COMPILATION_FAILED %s", infoLog);
        return 0;
    }

    Shader_LOGD("glCompileShader %d success", type);
    return sh;
}

int Shader::use() {
    GLint vsh = initShader(vertexShader, GL_VERTEX_SHADER);
    GLint fsh = initShader(fragmentShader, GL_FRAGMENT_SHADER);

    //创建渲染程序
    program = glCreateProgram();
    if (program == 0) {
        Shader_LOGD("glCreateProgram failed");
        return 0;
    }

    //向渲染程序中加入着色器
    glAttachShader(program, vsh);
    glAttachShader(program, fsh);

    //链接程序
    glLinkProgram(program);
    GLint status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == 0) {
        Shader_LOGD("glLinkProgram failed");
        return 0;
    }
    Shader_LOGD("glLinkProgram success");

    glDeleteShader(vsh);
    glDeleteShader(fsh);
    //激活渲染程序
    glUseProgram(program);
    return program;
}

Shader::Shader(const char *vertexShader, const char *fragmentShader) {
    this->vertexShader = vertexShader;
    this->fragmentShader = fragmentShader;
}

void Shader::release() {
    glDeleteProgram(program);
}


