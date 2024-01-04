//
// Created on 2023/12/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "http_curl.h"
#include "moonlight-core/moon_bridge.h"
#include <curl/easy.h>
#include <stdlib.h>
#include "string.h"
#include "napi/native_api.h"

struct AsyncCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    const char *url;
    const int timeout;
    const char *clientPath;
    const char *keyPath;
    char *result;
    const char *error;
};


size_t write_callback(void *contents, size_t size, size_t nmemb, void *output) {
    size_t total_size = size * nmemb;
    AsyncCallbackInfo *response_data = static_cast<AsyncCallbackInfo *>(output);
     if (contents == nullptr)
            return 0;
    size_t destSize = (response_data->result != nullptr) ? strlen(response_data->result) : 0;
    char* temp = new char[destSize + total_size + 1];
   
    if (response_data->result != nullptr) {
        memcpy(temp, response_data->result, destSize);
    }
    memcpy(temp + destSize, contents, total_size);
     temp[destSize + total_size] = '\0';
    delete[] response_data->result;
   
    response_data->result = temp;
    return total_size;
}

void getCurl(napi_env env, AsyncCallbackInfo* cb) {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        // 设置要访问的URL
        curl_easy_setopt(curl, CURLOPT_URL, cb->url);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, cb->timeout);
        curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
        if (cb->clientPath != nullptr)
            curl_easy_setopt(curl, CURLOPT_SSLCERT, cb->clientPath);
        if (cb->keyPath != nullptr){
            curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
            curl_easy_setopt(curl, CURLOPT_SSLKEY, cb->keyPath);
        }
            
        // 没有ca证书无法信任
        // 设置自签名证书的路径
        // curl_easy_setopt(curl, CURLOPT_CAINFO, "/data/storage/el2/base/haps/entry/cache/ca.pem");
        // 忽略SSL证书验证
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, cb);
        // 执行请求
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cb->error = curl_easy_strerror(res);
        }

        curl_easy_cleanup(curl);
    }
}

napi_value GetRequest(napi_env env, napi_callback_info info) {
    napi_deferred deferred;
    napi_value promise;
    napi_create_promise(env, &deferred, &promise);
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    char *url = get_value_string(env, args[0]);
    int timeout;
    napi_get_value_int32(env, args[1], &timeout);
    char *clientPath = get_value_string(env, args[2]);
    char *keyPath = get_value_string(env, args[3]);

    AsyncCallbackInfo *asyncCallbackInfo = new AsyncCallbackInfo{
        .env = env,
        .asyncWork = nullptr,
        .deferred = deferred,
        .url = url,
        .timeout = timeout,
        .clientPath = clientPath,
        .keyPath = keyPath,
        .result = nullptr
    };
    napi_value resourceName;
    napi_create_string_latin1(env, "GetRequest", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            getCurl(env, (AsyncCallbackInfo*)data);
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            if (asyncCallbackInfo-> error == nullptr) {
                napi_value result;
                napi_create_string_utf8(env, asyncCallbackInfo->result, strlen(asyncCallbackInfo->result), &result);
                // 触发回调
                napi_resolve_deferred(asyncCallbackInfo->env, asyncCallbackInfo->deferred, result);
            } else {
                napi_value result;
                napi_create_string_utf8(env, asyncCallbackInfo->error, NAPI_AUTO_LENGTH, &result);
                napi_reject_deferred(env, asyncCallbackInfo->deferred, result);
            }
            napi_delete_async_work(env, asyncCallbackInfo->asyncWork);
            delete asyncCallbackInfo;
        },
        (void *)asyncCallbackInfo, &asyncCallbackInfo->asyncWork);
    // 将异步工作排队，等待 Node.js 事件循环处理
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
    return promise;
}

static napi_value CurlClientClassConstructor(napi_env env, napi_callback_info info) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}
static napi_value Close(napi_env env, napi_callback_info info) {
    curl_global_cleanup();
    return 0;
}

void HttpCurlInit(napi_env env, napi_value exports) {
    napi_property_descriptor descriptors[] = {
        {"get", nullptr, GetRequest, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"close", nullptr, Close, nullptr, nullptr, nullptr, napi_default, nullptr}};
    napi_value result = nullptr;
    napi_define_class(env, "CurlClient", NAPI_AUTO_LENGTH, CurlClientClassConstructor, nullptr,
                      sizeof(descriptors) / sizeof(*descriptors), descriptors, &result);

    napi_set_named_property(env, exports, "CurlClient", result);
}
