//
// Created on 2023/12/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_moon_bridge_H
#define moonlight_moon_bridge_H
#include "napi/native_api.h"
#include "node_api.h"


void MoonBridgeJavascriptClassInit(napi_env env, napi_value exports);
char* get_value_string(napi_env env, napi_value value);

class MoonBridge{
    public:
       static void *nativewindow;
};

#endif //moonlight_moon_bridge_javascript_class_H
