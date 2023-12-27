//
// Created on 2023/12/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_AVFrameHolder_H
#define moonlight_AVFrameHolder_H
#include <mutex>

extern "C" {
#include "libavutil/frame.h"
#include <libavcodec/avcodec.h>
}

class AVFrameHolder {
  public:
    explicit AVFrameHolder();
    ~AVFrameHolder(){
    };
    static AVFrameHolder *GetInstance();
    
    void push(AVFrame *frame) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_frame = frame;
    }
    void get(const std::function<void(AVFrame *)> fn) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_frame) {
            fn(m_frame);
        }
    }

    void cleanup() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_frame = nullptr;
    }
  public:
    static AVFrameHolder* m_holder;
    std::mutex m_mutex;
    AVFrame *m_frame = nullptr;
};
#endif // moonlight_AVFrameHolder_H
