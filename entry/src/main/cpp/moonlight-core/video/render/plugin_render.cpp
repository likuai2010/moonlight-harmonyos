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

#include <stdint.h>
#include <string>
#include <js_native_api.h>
#include <js_native_api_types.h>
#include <hilog/log.h>
#include "moon_bridge.h"

#include "plugin_render.h"

std::unordered_map<std::string, PluginRender *> PluginRender::m_instance;
OH_NativeXComponent_Callback PluginRender::m_callback;

void OnSurfaceCreatedCB(OH_NativeXComponent *component, void *window)
{
    MoonBridgeApi::api->nativewindow = window;
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "Callback", "OnSurfaceCreatedCB");
    if ((nullptr == component) || (nullptr == window)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "Callback",
            "OnSurfaceCreatedCB: component or  window is null");
        return;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = { '\0' };
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentId(component, idStr, &idSize)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "Callback",
            "OnSurfaceCreatedCB: Unable to get XComponent id");
        return;
    }

    std::string id(idStr);
    auto render = PluginRender::GetInstance(id);
    uint64_t width;
    uint64_t height;
    int32_t xSize = OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    if ((OH_NATIVEXCOMPONENT_RESULT_SUCCESS == xSize) && (nullptr != render)) {
        DECODER_PARAMETERS params;
        params.context = window;
        params.width = 1280;
        params.height = 720;
//        if (render->m_eglCore->initialize(&params)) {
//            
//        }
    }
}

void OnSurfaceChangedCB(OH_NativeXComponent *component, void *window)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "Callback", "OnSurfaceChangedCB");
    if ((nullptr == component) || (nullptr == window)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "Callback",
            "OnSurfaceChangedCB: component or window is null");
        return;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = { '\0' };
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentId(component, idStr, &idSize)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "Callback",
            "OnSurfaceChangedCB: Unable to get XComponent id");
        return;
    }

    std::string id(idStr);
    auto render = PluginRender::GetInstance(id);
    if (nullptr != render) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "Callback", "surface changed");
    }
}

void OnSurfaceDestroyedCB(OH_NativeXComponent *component, void *window)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "Callback", "OnSurfaceDestroyedCB");
    if ((nullptr == component) || (nullptr == window)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "Callback",
            "OnSurfaceDestroyedCB: component or window is null");
        return;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = { '\0' };
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentId(component, idStr, &idSize)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "Callback",
            "OnSurfaceDestroyedCB: Unable to get XComponent id");
        return;
    }

    std::string id(idStr);
    PluginRender::Release(id);
}

void DispatchTouchEventCB(OH_NativeXComponent *component, void *window)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "Callback", "DispatchTouchEventCB");
    if ((nullptr == component) || (nullptr == window)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "Callback",
            "DispatchTouchEventCB: component or window is null");
        return;
    }
     uint64_t width, height;
     OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
     OH_NativeXComponent_TouchEvent touchEvent;
     OH_NativeXComponent_GetTouchEvent(component, window, &touchEvent);
     MoonBridge_sendTouchEvent(touchEvent, width, height);
}
void OnMouseEventCB(OH_NativeXComponent *component, void *window)
{
    if ((nullptr == component) || (nullptr == window)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "Callback",
            "DispatchTouchEventCB: component or window is null");
        return;
    }
    uint64_t width, height;
    OH_NativeXComponent_GetXComponentSize(component, window, &width, &height);
    OH_NativeXComponent_MouseEvent touchEvent;
    OH_NativeXComponent_GetMouseEvent(component, window, &touchEvent);
    MoonBridge_sendMouseEvent(touchEvent, width, height);
}
void OnHoverEventCB(OH_NativeXComponent *component, bool isHover)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "Callback", "OnHoverEventCB");
    if ((nullptr == component)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "Callback",
            "OnHoverEventCB: component or window is null");
        return;
    }
}
PluginRender::PluginRender(std::string &id)
{
    this->m_id = id;
    this->m_eglCore = new EglVideoRenderer();
    OH_NativeXComponent_Callback *renderCallback = &PluginRender::m_callback;
    renderCallback->OnSurfaceCreated = OnSurfaceCreatedCB;
    renderCallback->OnSurfaceChanged = OnSurfaceChangedCB;
    renderCallback->OnSurfaceDestroyed = OnSurfaceDestroyedCB;
    renderCallback->DispatchTouchEvent = DispatchTouchEventCB;
}

PluginRender *PluginRender::GetInstance(std::string &id)
{
    if (m_instance.find(id) == m_instance.end()) {
        PluginRender *instance = new PluginRender(id);
        m_instance[id] = instance;
        return instance;
    } else {
        return m_instance[id];
    }
}

void PluginRender::Export(napi_env env, napi_value exports)
{
    
    if ((nullptr == env) || (nullptr == exports)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "PluginRender", "Export: env or exports is null");
        return;
    }

    napi_property_descriptor desc[] = {
        { "drawRectangle", nullptr, PluginRender::NapiDrawRectangle, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    if (napi_ok != napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "PluginRender", "Export: napi_define_properties failed");
    }
}

// NAPI registration method type napi_callback. If no value is returned, nullptr is returned.
napi_value PluginRender::NapiDrawRectangle(napi_env env, napi_callback_info info)
{
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "PluginRender", "NapiDrawRectangle");
    if ((nullptr == env) || (nullptr == info)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "PluginRender", "NapiDrawRectangle: env or info is null");
        return nullptr;
    }

    napi_value thisArg;
    if (napi_ok != napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, nullptr)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "PluginRender", "NapiDrawRectangle: napi_get_cb_info fail");
        return nullptr;
    }

    napi_value exportInstance;
    if (napi_ok != napi_get_named_property(env, thisArg, OH_NATIVE_XCOMPONENT_OBJ, &exportInstance)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "PluginRender",
            "NapiDrawRectangle: napi_get_named_property fail");
        return nullptr;
    }

    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_ok != napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent))) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "PluginRender", "NapiDrawRectangle: napi_unwrap fail");
        return nullptr;
    }

    char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = { '\0' };
    uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
    if (OH_NATIVEXCOMPONENT_RESULT_SUCCESS != OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "PluginRender",
            "NapiDrawRectangle: Unable to get XComponent id");
        return nullptr;
    }

    std::string id(idStr);
    PluginRender *render = PluginRender::GetInstance(id);
    if (render) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "PluginRender", "render->m_eglCore->Draw() executed");
    }
    return nullptr;
}

void PluginRender::Release(std::string &id)
{
    PluginRender *render = PluginRender::GetInstance(id);
    if (nullptr != render) {
        render->m_eglCore->Release();
        delete render->m_eglCore;
        render->m_eglCore = nullptr;
        delete render;
        render = nullptr;
        m_instance.erase(m_instance.find(id));
    }
}