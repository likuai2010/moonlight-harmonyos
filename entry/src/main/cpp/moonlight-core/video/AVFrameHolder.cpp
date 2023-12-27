//
// Created on 2023/12/25.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include "video/AVFrameHolder.h"


AVFrameHolder::AVFrameHolder() {
    
}
AVFrameHolder* AVFrameHolder::m_holder = nullptr;

AVFrameHolder *AVFrameHolder::GetInstance() {
    if (m_holder == nullptr) {
        m_holder = new AVFrameHolder();
        return m_holder;
    } else {
        return m_holder;
    }
}