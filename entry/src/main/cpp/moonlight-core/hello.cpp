#include "napi/native_api.h"
#include "moon_bridge.h"
#include "utils/x509Utils.h"
#include "hilog/log.h"
#include <unistd.h>
#include "Shader.h"
#include "video/render/plugin_render.h"
#include "audio/Audio.h"
#include "audio/SDLAudioRenderer.h"

#define BITMAP_INFO_LOGD(...) OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "loadYuv", __VA_ARGS__)

static napi_value Add(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    double value0;
    napi_get_value_double(env, args[0], &value0);

    double value1;
    napi_get_value_double(env, args[1], &value1);

    napi_value sum;
    napi_create_double(env, value0 + value1, &sum);
    return sum;
}

struct BridgeCallbackInfo {
    EglVideoRenderer *render;

    napi_async_work asyncWork;
};
DataQueue *dataQueue = NULL;
Audio *audio = NULL;
SDLAudioRenderer* renderer = NULL;

static napi_value OpenSlEsPlayer_sendPcmData(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    void *message;
    size_t messageLength;
    napi_typedarray_type type;
    napi_get_typedarray_info(
        env,
        args[0],
        &type,
        &messageLength,
        &message,
        nullptr, // 可选的 ArrayBuffer
        nullptr  // 可选的偏移
    );

    if (audio == NULL) {
        audio = new Audio(dataQueue, 48000);
        audio->play();
    }
    
    PcmData *pdata = new PcmData((char *)(message), messageLength);
    dataQueue->putPcmData(pdata);
   
    return nullptr;
}

EXTERN_C_START

static napi_value Init(napi_env env, napi_value exports) {
    napi_status status;
    if (dataQueue == NULL) {
        dataQueue = new DataQueue();
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "testTag", "napi_define_properties %{public}s", "entry");

    MoonBridgeApi::api->Export(env, exports);

    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) {
    napi_module_register(&demoModule);
}
