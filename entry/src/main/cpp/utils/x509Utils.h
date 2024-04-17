//
// Created on 2023/12/8.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_x509Utils_H
#define moonlight_x509Utils_H
#include "node_api.h"

napi_value generate_certificate(napi_env env, napi_callback_info info);
napi_value verifySignature(napi_env env, napi_callback_info info);
napi_value getSignatureFromPemCert(napi_env env, napi_callback_info info);
napi_value signMessage(napi_env env, napi_callback_info info);
napi_value encrypt(napi_env env, napi_callback_info info);
napi_value decrypt(napi_env env, napi_callback_info info);
napi_value createTypedArray(napi_env env, size_t length, napi_typedarray_type type, void *data);

int generate_x509_certificate(char* cert_path, char* key_path);

#endif //moonlight_x509_H