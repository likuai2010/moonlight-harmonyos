//
// Created on 2023/12/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "libavcodec/avcodec.h"

void testFF(){
    avcodec_find_decoder_by_name("xxx");
}