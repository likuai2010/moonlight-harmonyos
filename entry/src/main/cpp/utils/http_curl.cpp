//
// Created on 2023/12/12.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "http_curl.h"
#include <curl/easy.h>
#include <stdlib.h>
#include "string.h"
#include "napi_utils.h"
#include "napi/native_api.h"
#include "x509Utils.h"
struct AsyncCallbackInfo {
    napi_env env;
    napi_async_work asyncWork;
    napi_deferred deferred;
    const char *url;
    const int timeout;
    const char *clientPath;
    const char *keyPath;
    void *result;
    size_t size;
    const char *error;
};

struct HTTP_DATA {
    char *memory;
    size_t size;
};

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    HTTP_DATA *mem = (HTTP_DATA *)userp;

    mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL)
        return 0;

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}
static CURL *curl;

int http_init( AsyncCallbackInfo *cb) {
    if (!curl) {
        curl_global_init(CURL_GLOBAL_ALL);
    } else {
        return 0;
    }

    curl = curl_easy_init();

    if (!curl)
        return 1;

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_SESSIONID_CACHE, 0L);

    return 0;
}
void http_request(AsyncCallbackInfo *cb) {

    HTTP_DATA *http_data = (HTTP_DATA *)malloc(sizeof(HTTP_DATA));
    http_data->memory = (char *)malloc(1);
    http_data->size = 0;

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, http_data);
    curl_easy_setopt(curl, CURLOPT_URL, cb->url);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, cb->timeout);
    if (cb->clientPath != nullptr) {
        curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
        curl_easy_setopt(curl, CURLOPT_SSLCERT, cb->clientPath);
    }
    if (cb->keyPath != nullptr) {
        curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
        curl_easy_setopt(curl, CURLOPT_SSLKEY, cb->keyPath);
    }

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        cb->error = curl_easy_strerror(res);
    } else if (http_data->memory == NULL) {
        cb->error = "Curl: memory = NULL";
    }

    cb->result = http_data->memory;
    cb->size = http_data->size;
    free(http_data->memory);
    free(http_data);
}

void getCurl(napi_env env, AsyncCallbackInfo *cb) {
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        // 设置要访问的URL
        curl_easy_setopt(curl, CURLOPT_URL, cb->url);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, cb->timeout);

        // 没有ca证书无法信任
        // 设置自签名证书的路径
        // curl_easy_setopt(curl, CURLOPT_CAINFO, "/data/storage/el2/base/haps/entry/cache/ca.pem");
        // 忽略SSL证书验证

//        if (cb->clientPath != nullptr) {
//            curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
//            curl_easy_setopt(curl, CURLOPT_SSLCERT, cb->clientPath);
//        }
//        if (cb->keyPath != nullptr) {
//            curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
//            curl_easy_setopt(curl, CURLOPT_SSLKEY, cb->keyPath);
//        }
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        HTTP_DATA *http_data = (HTTP_DATA *)malloc(sizeof(HTTP_DATA));
        http_data->memory = (char *)malloc(1);
        http_data->size = 0;

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, http_data);

        // 执行请求
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            cb->error = curl_easy_strerror(res);
        } else if (http_data->memory == NULL) {
            cb->error = "Curl: memory = NULL";
        }
        cb->result = http_data->memory;
        cb->size = http_data->size;
        free(http_data->memory);
        free(http_data);
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
        .result = nullptr};
    napi_value resourceName;
    napi_create_string_latin1(env, url, NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            http_request((AsyncCallbackInfo *)data);
            //getCurl(env, (AsyncCallbackInfo *)data);
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfo *asyncCallbackInfo = (AsyncCallbackInfo *)data;
            if (asyncCallbackInfo->error == nullptr) {
                napi_value result = createTypedArray(env, asyncCallbackInfo->size, napi_uint8_array, asyncCallbackInfo->result);
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
    napi_queue_async_work(env, asyncCallbackInfo->asyncWork);
    return promise;
}

static napi_value CurlClientClassConstructor(napi_env env, napi_callback_info info) {
    http_init(nullptr);
    napi_value thisArg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}
static napi_value Close(napi_env env, napi_callback_info info) {
    // curl_global_cleanup();
    
    return 0;
}

void HttpCurlInit(napi_env env, napi_value exports) {
    //curl_global_init(CURL_GLOBAL_DEFAULT);
    // char *clientPath = get_value_string(env, args[2]);
    //   char *keyPath = get_value_string(env, args[3]);
    napi_property_descriptor descriptors[] = {
        {"get", nullptr, GetRequest, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"close", nullptr, Close, nullptr, nullptr, nullptr, napi_default, nullptr}};
    napi_value result = nullptr;
    napi_define_class(env, "CurlClient", NAPI_AUTO_LENGTH, CurlClientClassConstructor, nullptr,
                      sizeof(descriptors) / sizeof(*descriptors), descriptors, &result);

    napi_set_named_property(env, exports, "CurlClient", result);
}
