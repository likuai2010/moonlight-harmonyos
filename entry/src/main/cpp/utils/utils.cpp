#include "utils/http_curl.h"
#include "napi/native_api.h"
#include "utils/x509Utils.h"
#include "hilog/log.h"

EXTERN_C_START

static napi_value Init(napi_env env, napi_value exports) {
    napi_status status;
    napi_property_descriptor desc[] = {
        {"generate_x509_certificate", nullptr, generate_certificate, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"verify_signature", nullptr, verifySignature, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"sign_message", nullptr, signMessage, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"decrypt", nullptr, decrypt, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"encrypt", nullptr, encrypt, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"get_signature_from_pemCert", nullptr, getSignatureFromPemCert, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "testTag", "napi_define_properties %{public}s", "cUtils");
    status = napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    HttpCurlInit(env, exports);
    return exports;
}
EXTERN_C_END

static napi_module utilsModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "cUtils",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterUtilsModule(void) {
    napi_module_register(&utilsModule);
}
