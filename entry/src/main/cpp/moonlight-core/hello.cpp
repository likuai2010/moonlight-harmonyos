#include "napi/native_api.h"
#include "moon_bridge_javascript_class.h"
#include "x509Utils.h"
#include <openssl/ssl.h>
#include <curl/curl.h>

static int testHttps();


static char* get_value_string(napi_env env, napi_value value){
     size_t length;
     napi_get_value_string_utf8(env, value, nullptr, 0, &length);
     char* buffer = (char*)malloc(length + 1);
     napi_get_value_string_utf8(env, value, buffer, length + 1, &length);
     return buffer;
}

// 回调函数，用于处理接收到的数据
size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append((char*)contents, total_size);
    return total_size;
}
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
    testHttps();
    return sum;
}
static int testHttps(){
        CURL *curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_DEFAULT);

        curl = curl_easy_init();
        if(curl) {
            // 设置要访问的URL
            curl_easy_setopt(curl, CURLOPT_URL, "https://192.168.3.5:47984/applist?uniqueid=0123456789ABCDEF&uuid=9983ad6c-0365-4f09-8085-9b3231eb4379&");
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L); 
            curl_easy_setopt(curl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_0);
            // 设置自签名证书的路径
            curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
            //curl_easy_setopt(curl, CURLOPT_CAINFO, "/data/storage/el2/base/haps/entry/cache/ca.pem");
            curl_easy_setopt(curl, CURLOPT_SSLCERT, "/data/storage/el2/base/haps/entry/cache/client.pem");
            curl_easy_setopt(curl, CURLOPT_SSLKEY, "/data/storage/el2/base/haps/entry/cache/private.key");
             //忽略SSL证书验证
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
            // 执行请求
            res = curl_easy_perform(curl);

   //          检查执行结果
            if(res != CURLE_OK){
               const char* err = curl_easy_strerror(res);
               fprintf(stderr, "curl_easy_perform() failed: %s\n", err);
            }

     //        清理资源
            curl_easy_cleanup(curl);
        }

        curl_global_cleanup();

        return 0;
}




static napi_value generate_certificate(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value args[2] = {nullptr};

    napi_get_cb_info(env, info, &argc, args , nullptr, nullptr);

    char* certPath = get_value_string(env, args[0]);
    char* keyPath = get_value_string(env, args[1]);
    generate_x509_certificate(certPath, keyPath);
    return 0;
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
