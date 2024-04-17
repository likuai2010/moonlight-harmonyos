//
// Created on 2024/4/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "napi_utils.h"
#include <string>

char *get_value_string(napi_env env, napi_value value) {
    size_t length;
    napi_get_value_string_utf8(env, value, nullptr, 0, &length);
    char *buffer = (char *)malloc(length + 1);
    napi_get_value_string_utf8(env, value, buffer, length + 1, &length);
    return buffer;
}
