//
// Created on 2023/12/5.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "napi/native_api.h"

#include <pthread.h>
#include <string.h>
#include <Limelight.h>

// 解码
// #include <opus_multistream.h>

static pthread_key_t napiEnvKey;
static pthread_once_t JniEnvKeyInitOnce = PTHREAD_ONCE_INIT;

// static JavaVM *JVM;

void DetachThread(napi_env env) {
    // 
    //(*JVM)->DetachCurrentThread(JVM);
}
void JniEnvKeyInit(void) {
    // Create a TLS slot for the JNIEnv. We aren't in
    // a pthread during init, so we must wait until we
    // are to initialize this.
    pthread_key_create(&napiEnvKey, DetachThread);
}
napi_env *GetThreadEnv(void) {
    napi_env *env;
    
    // Create the TLS slot now that we're safely in a pthread
    pthread_once(&JniEnvKeyInitOnce, JniEnvKeyInit);
    
    // Try the TLS to see if we already have a JNIEnv
    env = pthread_getspecific(napiEnvKey);
    if (env)
        return env;
    // This is the thread's first JNI call, so attach now
    //(*JVM)->AttachCurrentThread(JVM, &env, NULL);

    // Write our JNIEnv to TLS, so we detach before dying
    pthread_setspecific(napiEnvKey, env);
}