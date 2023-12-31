//
// Created on 2023/12/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "moon_bridge.h"
#include <native_window/external_window.h>
#include <video/FFmpegVideoDecoder.h>

#define NDEBUG
#include <Limelight.h>
#include "napi/native_api.h"
#include <hilog/log.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "video/render/plugin_render.h"
#include "video/AVFrameHolder.h"
#include <unistd.h>

static FFmpegVideoDecoder *m_decoder = nullptr;

void *MoonBridge::nativewindow = nullptr;
static FILE *fo = fopen("/data/storage/el2/base/haps/entry/cache/stream.yuv", "wb+");

static napi_value MoonBridge_startConnection(napi_env env, napi_callback_info info);

int BridgeDrSetup(int videoFormat, int width, int height, int redrawRate, void *context, int drFlags) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "testTag", "BridgeDrSetup");
    DECODER_PARAMETERS param;
    param.video_format = videoFormat;
    param.width = width;
    param.height = height;
    param.context = MoonBridge::nativewindow;
    param.frame_rate = redrawRate;
    param.dr_flags = drFlags;
    return m_decoder->setup(&param);
}

void BridgeDrStart(void) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "testTag", "BridgeDrStart");
    m_decoder->start();
}

void BridgeDrStop(void) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "testTag", "BridgeDrStop");
    m_decoder->stop();
}

void BridgeDrCleanup(void) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "testTag", "BridgeDrCleanup");
    m_decoder->cleanup();
}

int BridgeDrSubmitDecodeUnit(PDECODE_UNIT decodeUnit) {
    return m_decoder->submitDecodeUnit(decodeUnit);
}

int BridgeArInit(int audioConfiguration, POPUS_MULTISTREAM_CONFIGURATION opusConfig, void *context, int flags) {
    return 0;
}

void BridgeArStart(void) {
}

void BridgeArStop(void) {
}

void BridgeArCleanup() {
}

void BridgeArDecodeAndPlaySample(char *sampleData, int sampleLength) {
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "testTag", "BridgeArDecodeAndPlaySample");
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
    .capabilities = CAPABILITY_SUPPORTS_ARBITRARY_AUDIO_DURATION};
void BridgeClLogMessage(const char *format, ...) {
    va_list va;
    va_start(va, format);
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "testTag", format, va);
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

char *get_value_string(napi_env env, napi_value value) {
    size_t length;
    napi_get_value_string_utf8(env, value, nullptr, 0, &length);
    char *buffer = (char *)malloc(length + 1);
    napi_get_value_string_utf8(env, value, buffer, length + 1, &length);
    return buffer;
}

struct BridgeCallbackInfo {
    SERVER_INFORMATION serverInfo;
    STREAM_CONFIGURATION streamConfig;
    EglVideoRenderer* render;
    napi_async_work asyncWork;
};

static napi_value MoonBridge_startConnection(napi_env env, napi_callback_info info) {
    size_t argc = 20;
    napi_value args[20] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

    char *address;        // 假设字符串不超过 256 个字符
    char *appVersion;     // 假设字符串不超过 256 个字符
    char *gfeVersion;     // 假设字符串不超过 256 个字符
    char *rtspSessionUrl; // 假设字符串不超过 256 个字符
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
    void *riAesKey;
    void *riAesIv;
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
    size_t length;
    napi_typedarray_type arrayType;
    napi_get_typedarray_info(
        env,
        args[15],
        &arrayType,
        &length,
        &riAesKey,
        nullptr, // 可选的 ArrayBuffer
        nullptr  // 可选的偏移
    );
    size_t riAesIvLength;
    napi_get_typedarray_info(
        env,
        args[16],
        &arrayType,
        &riAesIvLength,
        &riAesIv,
        nullptr, // 可选的 ArrayBuffer
        nullptr  // 可选的偏移
    );
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
        .colorRange = colorRange};

    memcpy(streamConfig.remoteInputAesKey, riAesKey, sizeof(streamConfig.remoteInputAesKey));
    memcpy(streamConfig.remoteInputAesIv, riAesIv, sizeof(streamConfig.remoteInputAesIv));

    BridgeVideoRendererCallbacks.capabilities = videoCapabilities;
   
     EglVideoRenderer *render = new EglVideoRenderer();
    BridgeCallbackInfo *bridgeCallbackInfo = new BridgeCallbackInfo{
        .serverInfo = serverInfo,
        .streamConfig = streamConfig,
        .render = render
    };
    napi_value resourceName;
    napi_create_string_latin1(env, "GetRequest", NAPI_AUTO_LENGTH, &resourceName);
     
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            BridgeCallbackInfo *info = (BridgeCallbackInfo *)data;
            int ret = LiStartConnection(&info->serverInfo,
                                        &info->streamConfig,
                                        &BridgeConnListenerCallbacks,
                                        &BridgeVideoRendererCallbacks,
                                        &BridgeAudioRendererCallbacks,
                                        nullptr, 0,
                                        nullptr, 0);
            OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "testTag", "start con -> %{public}d", ret);
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
    // 将异步工作排队，等待 Node.js 事件循环处理
    napi_queue_async_work(env, bridgeCallbackInfo->asyncWork);
    napi_value result;
    napi_create_int32(env, -99, &result);
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
static napi_value MoonBridgeJavascriptClassConstructor(napi_env env, napi_callback_info info) {
    napi_value thisArg = nullptr;
    void *data = nullptr;
    int vale = 1;
    BridgeClLogMessage("MoonBridgeJavascriptClassConstructor xxx %d\n", vale);
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);

    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}

static OH_NativeXComponent_Callback callback;

void MoonBridgeJavascriptClassInit(napi_env env, napi_value exports) {
    // 需要 api 9 没有真机测试
    // m_decoder = (IVideoDecoder *)new NativeVideoDecoder();
    // 软解码 ffmpeg cpu
    m_decoder = new FFmpegVideoDecoder();

    napi_property_descriptor descriptors[] = {
        {"startConnection", nullptr, MoonBridge_startConnection, nullptr, nullptr, nullptr, napi_default, nullptr}};
    napi_value result = nullptr;

    napi_define_class(env, "MoonBridgeNapi", NAPI_AUTO_LENGTH, MoonBridgeJavascriptClassConstructor, nullptr,
                      sizeof(descriptors) / sizeof(*descriptors), descriptors, &result);

    napi_set_named_property(env, exports, "MoonBridgeNapi", result);

    napi_value exportInstance;
    if (napi_get_named_property(env, exports, "__NATIVE_XCOMPONENT_OBJ__", &exportInstance) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0, "PluginManager", "Export: napi_get_named_property fail");
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0, "PluginManager", "napi_get_named_property success");
    }
    OH_NativeXComponent *nativeXComponent = nullptr;
    if (napi_unwrap(env, exportInstance, reinterpret_cast<void **>(&nativeXComponent)) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0, "PluginManager", "Export: napi_unwrap fail");
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0, "PluginManager", "Export: napi_unwrap success");
        char idStr[OH_XCOMPONENT_ID_LEN_MAX + 1] = {'\0'};
        uint64_t idSize = OH_XCOMPONENT_ID_LEN_MAX + 1;
        if (OH_NativeXComponent_GetXComponentId(nativeXComponent, idStr, &idSize) != OH_NATIVEXCOMPONENT_RESULT_SUCCESS) {
            OH_LOG_Print(
                LOG_APP, LOG_ERROR, 0, "PluginManager", "Export: OH_NativeXComponent_GetXComponentId fail");
            return;
        }
        OH_LOG_Print(
            LOG_APP, LOG_ERROR, 0, "PluginManager", "GetXComponentId %{public}s", idStr);

        if (nativeXComponent != nullptr) {
            int eer = OH_NativeXComponent_RegisterCallback(nativeXComponent, &PluginRender::m_callback);
            OH_LOG_Print(
                LOG_APP, LOG_ERROR, 0, "PluginManager", "RegisterCallback success %{public}d", eer);
        }
        std::string id(idStr);
        PluginRender *pr = PluginRender::GetInstance(id);

        pr->Export(env, exports);
    }
}
