//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_napi_utils.h_H
#define moonlight_napi_utils.h_H
#include "node_api.h"
char *get_value_string(napi_env env, napi_value value);

#endif //moonlight_napi_utils.h_H
