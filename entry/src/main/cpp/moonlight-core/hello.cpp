#include "moonlight-core/http_curl.h"
#include "napi/native_api.h"
#include "moon_bridge.h"
#include "x509Utils.h"





static napi_value Add(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value args[3] = {nullptr};

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    double value0;
    napi_get_value_double(env, args[0], &value0);

    double value1;
    napi_get_value_double(env, args[1], &value1);

    napi_value sum;
    napi_create_double(env, value0 + value1, &sum);
    return sum;
}






EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports)
{
    napi_status status;
    
    napi_property_descriptor desc[] = {
        { "add", nullptr, Add, nullptr, nullptr, nullptr, napi_default, nullptr },
        { "generate_x509_certificate", nullptr, generate_certificate, nullptr, nullptr, nullptr, napi_default, nullptr },
    };
    
    status = napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    if(status != napi_ok)
        return NULL;
    
    MoonBridgeJavascriptClassInit(env, exports);
    HttpCurlInit(env, exports);
    
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version =1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "entry",
    .nm_priv = ((void*)0),
    .reserved = { 0 },
};


extern "C" __attribute__((constructor)) void RegisterEntryModule(void)
{
    napi_module_register(&demoModule);
}
