//
// Created on 2023/12/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_http_curl_H
#define moonlight_http_curl_H
#include "node_api.h"
#include <curl/curl.h>

void HttpCurlInit(napi_env env, napi_value exports);

#endif //moonlight_http_curl.h_H
