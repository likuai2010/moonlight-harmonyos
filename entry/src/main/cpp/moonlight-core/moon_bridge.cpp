//
// Created on 2023/12/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "moon_bridge_javascript_class.h"
#include <Limelight.h>
#include "napi/native_api.h"
#include <memory>
#include <hilog/log.h>
#include <stdio.h>
static napi_value MoonBridge_startConnection(napi_env env, napi_callback_info info);




int BridgeDrSetup(int videoFormat, int width, int height, int redrawRate, void* context, int drFlags) {
}

void BridgeDrStart(void) {
}

void BridgeDrStop(void) {
}

void BridgeDrCleanup(void) {
}

int BridgeDrSubmitDecodeUnit(PDECODE_UNIT decodeUnit) {
}

int BridgeArInit(int audioConfiguration, POPUS_MULTISTREAM_CONFIGURATION opusConfig, void* context, int flags) {
}

void BridgeArStart(void) {
}

void BridgeArStop(void) {
}

void BridgeArCleanup() {
}

void BridgeArDecodeAndPlaySample(char* sampleData, int sampleLength) {
}

void BridgeClStageStarting(int stage) {
}

void BridgeClStageComplete(int stage) {
}

void BridgeClStageFailed(int stage, int errorCode) {
}

void BridgeClConnectionStarted(void) {
}

void BridgeClConnectionTerminated(int errorCode) {
}

void BridgeClRumble(unsigned short controllerNumber, unsigned short lowFreqMotor, unsigned short highFreqMotor) {
}

void BridgeClConnectionStatusUpdate(int connectionStatus) {
}

void BridgeClSetHdrMode(bool enabled) {
}

void BridgeClRumbleTriggers(unsigned short controllerNumber, unsigned short leftTrigger, unsigned short rightTrigger) {
}

void BridgeClSetMotionEventState(uint16_t controllerNumber, uint8_t motionType, uint16_t reportRateHz) {
}

void BridgeClSetControllerLED(uint16_t controllerNumber, uint8_t r, uint8_t g, uint8_t b) {
   
}
static DECODER_RENDERER_CALLBACKS BridgeVideoRendererCallbacks = {
        .setup = BridgeDrSetup,
        .start = BridgeDrStart,
        .stop = BridgeDrStop,
        .cleanup = BridgeDrCleanup,
        .submitDecodeUnit = BridgeDrSubmitDecodeUnit,
};

static AUDIO_RENDERER_CALLBACKS BridgeAudioRendererCallbacks = {
        .init = BridgeArInit,
        .start = BridgeArStart,
        .stop = BridgeArStop,
        .cleanup = BridgeArCleanup,
        .decodeAndPlaySample = BridgeArDecodeAndPlaySample,
        .capabilities = CAPABILITY_SUPPORTS_ARBITRARY_AUDIO_DURATION
};
void BridgeClLogMessage(const char* format, ...) {
    va_list va;
    va_start(va, format);
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "moonlight-common-c", format, va);
    va_end(va);
}

static CONNECTION_LISTENER_CALLBACKS BridgeConnListenerCallbacks = {
        .stageStarting = BridgeClStageStarting,
        .stageComplete = BridgeClStageComplete,
        .stageFailed = BridgeClStageFailed,
        .connectionStarted = BridgeClConnectionStarted,
        .connectionTerminated = BridgeClConnectionTerminated,
        .logMessage = BridgeClLogMessage,
        .rumble = BridgeClRumble,
        .connectionStatusUpdate = BridgeClConnectionStatusUpdate,
        .setHdrMode = BridgeClSetHdrMode,
        .rumbleTriggers = BridgeClRumbleTriggers,
        .setMotionEventState = BridgeClSetMotionEventState,
        .setControllerLED = BridgeClSetControllerLED,
};


static char* get_value_string(napi_env env, napi_value value){
     size_t length;
     napi_get_value_string_utf8(env, value, nullptr, 0, &length);
     char* buffer = (char*)malloc(length + 1);
     napi_get_value_string_utf8(env, value, buffer, length + 1, &length);
     return buffer;
}

static napi_value MoonBridge_startConnection(napi_env env, napi_callback_info info) {
    size_t argc = 20;
    napi_value args[20] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    
    char* address;  // 假设字符串不超过 256 个字符
    char* appVersion;  // 假设字符串不超过 256 个字符
    char* gfeVersion;  // 假设字符串不超过 256 个字符
    char* rtspSessionUrl;  // 假设字符串不超过 256 个字符
    int serverCodecModeSupport;
    int width; 
    int height;
    int fps;
    int bitrate;
    int packetSize;
    int streamingRemotely;
    int audioConfiguration;
    int supportedVideoFormats;
    int clientRefreshRateX100;
    int encryptionFlags;
    void* riAesKey;
    void* riAesIv;
    int videoCapabilities;
    int colorSpace;
    int colorRange;
    
    address = get_value_string(env, args[0]);
    appVersion = get_value_string(env, args[1]);
    gfeVersion = get_value_string(env, args[2]);
    rtspSessionUrl = get_value_string(env, args[3]);
    napi_get_value_int32(env, args[4], &serverCodecModeSupport);
    napi_get_value_int32(env, args[5], &width);
    napi_get_value_int32(env, args[6], &height);
    napi_get_value_int32(env, args[7], &fps);
    napi_get_value_int32(env, args[8], &bitrate);
    napi_get_value_int32(env, args[9], &packetSize);
    napi_get_value_int32(env, args[10], &streamingRemotely);
    napi_get_value_int32(env, args[11], &audioConfiguration);
    napi_get_value_int32(env, args[12], &supportedVideoFormats);
    napi_get_value_int32(env, args[13], &clientRefreshRateX100);
    napi_get_value_int32(env, args[14], &encryptionFlags);
    size_t riAesKeyLength;
   // napi_get_buffer_info(env, args[15], &riAesKey, &riAesKeyLength);
    size_t riAesIvLength;
   // napi_get_buffer_info(env, args[16], &riAesIv, &riAesIvLength);
    napi_get_value_int32(env, args[17], &videoCapabilities);
    napi_get_value_int32(env, args[18], &colorSpace);
    napi_get_value_int32(env, args[19], &colorRange);
    
    SERVER_INFORMATION serverInfo = {
            .address = address,
            .serverInfoAppVersion = appVersion,
            .serverInfoGfeVersion = gfeVersion,
            .rtspSessionUrl = rtspSessionUrl,
            .serverCodecModeSupport = serverCodecModeSupport,
    };
    STREAM_CONFIGURATION streamConfig = {
            .width = width,
            .height = height,
            .fps = fps,
            .bitrate = bitrate,
            .packetSize = packetSize,
            .streamingRemotely = streamingRemotely,
            .audioConfiguration = audioConfiguration,
            .supportedVideoFormats = supportedVideoFormats,
            .clientRefreshRateX100 = clientRefreshRateX100,
            .encryptionFlags = encryptionFlags,
            .colorSpace = colorSpace,
            .colorRange = colorRange
    };

   // memcpy(streamConfig.remoteInputAesKey, riAesKey, sizeof(streamConfig.remoteInputAesKey));
   // memcpy(streamConfig.remoteInputAesIv, riAesIv, sizeof(streamConfig.remoteInputAesIv));

    BridgeVideoRendererCallbacks.capabilities = videoCapabilities;

    int ret = LiStartConnection(&serverInfo,
                                &streamConfig,
                                &BridgeConnListenerCallbacks,
                                &BridgeVideoRendererCallbacks,
                                &BridgeAudioRendererCallbacks,
                                nullptr, 0,
                                nullptr, 0);
    napi_value result;
    napi_create_int32(env, ret, &result);
    return result;
}


enum TestEnum {
    ONE = 0,
    TWO,
    THREE,
    FOUR
};

/*
 * Constructor
 */
static napi_value MoonBridgeJavascriptClassConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisArg = nullptr;
    void* data = nullptr;
    int vale = 1;
    BridgeClLogMessage("MoonBridgeJavascriptClassConstructor xxx %d\n", vale);
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);

    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}

void MoonBridgeJavascriptClassInit(napi_env env, napi_value exports)
{
    napi_value one = nullptr;
    napi_value two = nullptr;
    napi_value three = nullptr;
    napi_value four = nullptr;

    napi_create_int32(env, TestEnum::ONE, &one);
    napi_create_int32(env, TestEnum::TWO, &two);
    napi_create_int32(env, TestEnum::THREE, &three);
    napi_create_int32(env, TestEnum::FOUR, &four);

    napi_property_descriptor descriptors[] = {
        { "startConnection", nullptr, MoonBridge_startConnection, nullptr, nullptr, nullptr, napi_default, nullptr }
    };
    napi_value result = nullptr;
    napi_define_class(env, "MoonBridgeNapi", NAPI_AUTO_LENGTH, MoonBridgeJavascriptClassConstructor, nullptr,
            sizeof(descriptors) / sizeof(*descriptors), descriptors, &result);

    napi_set_named_property(env, exports, "MoonBridgeNapi", result);
}


