#include "utils/http_curl.h"
#include "napi/native_api.h"
#include "moon_bridge.h"
#include "utils/x509Utils.h"
#include "hilog/log.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <fstream>
#include <math.h>
#include <unistd.h>
#include "Shader.h"
#include "FragmentShader.h"
#include "video/render/plugin_render.h"
#include "video/common/common.h"
#include "video/AVFrameHolder.h"
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

static napi_value loadYuv(napi_env env, napi_callback_info info) {
    EglVideoRenderer *render = new EglVideoRenderer();

    BridgeCallbackInfo *bridgeCallbackInfo = new BridgeCallbackInfo{
        .render = render};
    napi_value resourceName;
    napi_create_string_latin1(env, "GetRequest", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            BridgeCallbackInfo *info = (BridgeCallbackInfo *)data;
            DECODER_PARAMETERS params;
            params.context = MoonBridge::nativewindow;
            params.width = 1280;
            params.height = 720;
            info->render->initialize(&params);
            while (true) {
                AVFrameHolder::GetInstance()->get([info](AVFrame *frame) {
                    info->render->renderFrame(frame);
                });
                usleep(100000 / 120);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            BridgeCallbackInfo *info = (BridgeCallbackInfo *)data;
            napi_delete_async_work(env, info->asyncWork);
            delete info;
        },
        (void *)bridgeCallbackInfo, &bridgeCallbackInfo->asyncWork);
    napi_queue_async_work(env, bridgeCallbackInfo->asyncWork);

    return nullptr;
}

EXTERN_C_START
napi_value GetContext(napi_env env, napi_callback_info info) {
    if ((env == nullptr) || (info == nullptr)) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "PluginManager", "GetContext env or info is null");
        return nullptr;
    }

    size_t argCnt = 1;
    napi_value args[1] = {nullptr};
    if (napi_get_cb_info(env, info, &argCnt, args, nullptr, nullptr) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "PluginManager", "GetContext napi_get_cb_info failed");
    }

    if (argCnt != 1) {
        napi_throw_type_error(env, NULL, "Wrong number of arguments");
        return nullptr;
    }

    napi_valuetype valuetype;
    if (napi_typeof(env, args[0], &valuetype) != napi_ok) {
        napi_throw_type_error(env, NULL, "napi_typeof failed");
        return nullptr;
    }

    if (valuetype != napi_number) {
        napi_throw_type_error(env, NULL, "Wrong type of arguments");
        return nullptr;
    }

    int64_t value;
    if (napi_get_value_int64(env, args[0], &value) != napi_ok) {
        napi_throw_type_error(env, NULL, "napi_get_value_int64 failed");
        return nullptr;
    }

    napi_value exports;
    if (napi_create_object(env, &exports) != napi_ok) {
        napi_throw_type_error(env, NULL, "napi_create_object failed");
        return nullptr;
    }

    return exports;
}

static napi_value Init(napi_env env, napi_value exports) {
    napi_status status;

    napi_property_descriptor desc[] = {
        {"add", nullptr, Add, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"loadYuv", nullptr, loadYuv, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"getContext", nullptr, GetContext, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"generate_x509_certificate", nullptr, generate_certificate, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"verify_signature", nullptr, verifySignature, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"sign_message", nullptr, signMessage, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"decrypt", nullptr, decrypt, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"encrypt", nullptr, encrypt, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"get_signature_from_pemCert", nullptr, getSignatureFromPemCert, nullptr, nullptr, nullptr, napi_default, nullptr},
    };

    status = napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    if (status != napi_ok)
        return NULL;

    MoonBridgeJavascriptClassInit(env, exports);
    HttpCurlInit(env, exports);

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
