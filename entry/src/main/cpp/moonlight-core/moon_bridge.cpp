//
// Created on 2023/12/6.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "moon_bridge.h"
#include <audio/SDLAudioRenderer.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <native_window/external_window.h>
#include <video/FFmpegVideoDecoder.h>
#define NDEBUG
#include <Limelight.h>
#include "napi/native_api.h"
#include <hilog/log.h>
#include <ace/xcomponent/native_interface_xcomponent.h>
#include "video/AVFrameHolder.h"
#include "video/NativeVideoDecoder.h"
#include <unistd.h>
#include <arpa/inet.h>

MoonBridgeApi *MoonBridgeApi::api = new MoonBridgeApi();

MoonBridgeApi::MoonBridgeApi() {
#ifdef FFMPEG_ENABLED
    m_decoder = new FFmpegVideoDecoder();
#endif
    if(NativeVideoDecoder::supportedHW()){
        m_decoder = new NativeVideoDecoder();
    }
    m_audioRender = new SDLAudioRenderer();
    m_videoRender = new EglVideoRenderer();
    BridgeVideoRendererCallbacks = {
        .setup = BridgeDrSetup,
        .start = BridgeDrStart,
        .stop = BridgeDrStop,
        .cleanup = BridgeDrCleanup,
        .submitDecodeUnit = BridgeDrSubmitDecodeUnit,
    };
    BridgeAudioRendererCallbacks = {
        .init = BridgeArInit,
        .start = BridgeArStart,
        .stop = BridgeArStop,
        .cleanup = BridgeArCleanup,
        .decodeAndPlaySample = BridgeArDecodeAndPlaySample,
        .capabilities = CAPABILITY_SUPPORTS_ARBITRARY_AUDIO_DURATION};
    BridgeConnListenerCallbacks = {
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
}

napi_value ConvertFloatToNapiValue(napi_env env, float intValue) {
    napi_value result;
    napi_status status;
    status = napi_create_double(env, intValue, &result);
    if (status != napi_ok) {
        // Handle error
        return NULL;
    }
    return result;
}
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
    EglVideoRenderer *render;
    napi_async_work asyncWork;
};
napi_value MoonBridgeApi::MoonBridge_startConnection(napi_env env, napi_callback_info info) {
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

    api->BridgeVideoRendererCallbacks.capabilities = videoCapabilities;
    int ret = LiStartConnection(&serverInfo,
                                &streamConfig,
                                &api->BridgeConnListenerCallbacks,
                                &api->BridgeVideoRendererCallbacks,
                                &api->BridgeAudioRendererCallbacks,
                                api->nativewindow, 0,
                                nullptr, 0);
    EglVideoRenderer *render = new EglVideoRenderer();
    BridgeCallbackInfo *bridgeCallbackInfo = new BridgeCallbackInfo{
        .serverInfo = serverInfo,
        .streamConfig = streamConfig,
        .render = render};
    napi_value resourceName;
    napi_create_string_latin1(env, "GetRequest", NAPI_AUTO_LENGTH, &resourceName);
    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            BridgeCallbackInfo *info = (BridgeCallbackInfo *)data;
            if(api->m_decoder->getParams() != NULL){
                info->render->initialize(api->m_decoder->getParams());
                while (true) {
                    AVFrameHolder::GetInstance()->get([info](AVFrame *frame) { info->render->renderFrame(frame); });
                    usleep(100000 / 120);
                }
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
    napi_create_int32(env, ret, &result);
    return result;
}
napi_value MoonBridgeApi::MoonBridge_stopConnection(napi_env env, napi_callback_info info) {
    LiStopConnection();
    return nullptr;
}
napi_value MoonBridgeApi::MoonBridge_interruptConnection(napi_env env, napi_callback_info info) {
    LiInterruptConnection();
    return nullptr;
}

static void Napi_OnVideoStatus(napi_env env, napi_value js_callback, void *context, void *data) {
    VIDEO_STATS *status = (VIDEO_STATS *)data;
    if(status == nullptr)
        return ;
    napi_value params[1];
    napi_value stats;
    napi_create_object(env, &stats);
    napi_set_named_property(env, stats, "decodedFps", ConvertFloatToNapiValue(env, status->decodedFps));
    napi_set_named_property(env, stats, "receivedFps", ConvertFloatToNapiValue(env, status->receivedFps));
    napi_set_named_property(env, stats, "renderedFps", ConvertFloatToNapiValue(env, status->renderedFps));
    napi_set_named_property(env, stats, "totalFps", ConvertFloatToNapiValue(env, status->totalFps));
    napi_set_named_property(env, stats, "networkDroppedRate", ConvertFloatToNapiValue(env, status->networkDroppedFrames / status->totalFrames * 100));
    napi_set_named_property(env, stats, "networkDroppedFrames", ConvertFloatToNapiValue(env, status->networkDroppedFrames));
    napi_set_named_property(env, stats, "decodeTime", ConvertFloatToNapiValue(env, status->totalDecodeTime / status->decodedFrames));
    napi_set_named_property(env, stats, "receivedTime", ConvertFloatToNapiValue(env, status->totalReassemblyTime / status->receivedFrames));
    params[0] = stats;
    napi_call_function(env, nullptr, js_callback, 1, params, nullptr);
}

static napi_value MoonBridge_onVideoStatus(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    napi_value resourceName;
    napi_create_string_latin1(env, "onVideoStatus", NAPI_AUTO_LENGTH, &resourceName);
    napi_threadsafe_function tsfn;
    napi_create_threadsafe_function(env, args[0], NULL, resourceName, 0, 1, NULL, NULL, NULL, Napi_OnVideoStatus, &tsfn);
    MoonBridgeApi::api->setFunByName("OnVideoStatus", tsfn);
    return nullptr;
}

// <limelight.h>
uint8_t getTouchEvent(OH_NativeXComponent_TouchEventType type) {
    uint8_t eventType;
    switch (type) {
    case OH_NATIVEXCOMPONENT_DOWN:
        eventType = LI_TOUCH_EVENT_DOWN;
        break;
    case OH_NATIVEXCOMPONENT_UP:
        eventType = LI_TOUCH_EVENT_UP;
        break;
    case OH_NATIVEXCOMPONENT_MOVE:
        eventType = LI_TOUCH_EVENT_MOVE;
        break;
    case OH_NATIVEXCOMPONENT_CANCEL:
        eventType = LI_TOUCH_EVENT_CANCEL_ALL;
        break;
    default:
        LI_ROT_UNKNOWN;
    }
    return eventType;
}

int MoonBridge_sendTouchEvent(
    OH_NativeXComponent_TouchEvent touchEvent,
    uint64_t width, uint64_t height) {
    uint8_t eventType = getTouchEvent(touchEvent.type);
    uint32_t pointerId;
    float x, y;
    int ret = 0;
    float pressureOrDistance = touchEvent.force;
    float contactAreaMajor = 0.0f;
    float contactAreaMinor = 0.0f;
    float rotation = LI_ROT_UNKNOWN;
    if (touchEvent.type == OH_NATIVEXCOMPONENT_MOVE) {
        for (int i = 0; i < touchEvent.numPoints; i++) {
            pointerId = touchEvent.touchPoints[i].id;
            pressureOrDistance = touchEvent.touchPoints[i].force;
            if (touchEvent.touchPoints[i].isPressed) {
                LiSendTouchEvent(eventType, pointerId, touchEvent.touchPoints[i].x / width, touchEvent.touchPoints[i].y / height, pressureOrDistance, contactAreaMajor, contactAreaMinor, rotation);
            } else {
            }
        }
        ret = 0;
    } else if (touchEvent.type == OH_NATIVEXCOMPONENT_CANCEL) {
        ret = LiSendTouchEvent(LI_TOUCH_EVENT_CANCEL_ALL, 0, 0, 0, 0, 0, 0, rotation);
    } else {
        pointerId = touchEvent.touchPoints[0].id;
        x = touchEvent.touchPoints[0].x / width;
        y = touchEvent.touchPoints[0].y / height;
        pressureOrDistance = touchEvent.touchPoints[0].force;
        ret = LiSendTouchEvent(eventType, pointerId, x, y, pressureOrDistance, contactAreaMajor, contactAreaMinor, rotation);
    }
    return ret;
}
int MoonBridge_sendMouseEvent(
    OH_NativeXComponent_MouseEvent mouseEvent,
    uint64_t width, uint64_t height) {
    uint8_t button;
    switch (mouseEvent.button) {
    case OH_NATIVEXCOMPONENT_LEFT_BUTTON:
        button = BUTTON_LEFT;
    case OH_NATIVEXCOMPONENT_RIGHT_BUTTON:
        button = BUTTON_RIGHT;
    case OH_NATIVEXCOMPONENT_MIDDLE_BUTTON:
        button = BUTTON_MIDDLE;
    case OH_NATIVEXCOMPONENT_BACK_BUTTON:
        button = BUTTON_X1;
    case OH_NATIVEXCOMPONENT_FORWARD_BUTTON:
        button = BUTTON_X2;
    }
    if (mouseEvent.action == OH_NATIVEXCOMPONENT_MOUSE_MOVE) {
        LiSendMousePositionEvent(mouseEvent.x, mouseEvent.y, width, height);
    }
    if (mouseEvent.action == OH_NATIVEXCOMPONENT_MOUSE_PRESS) {
        LiSendMouseButtonEvent(BUTTON_ACTION_PRESS, button);
    }
    if (mouseEvent.action == OH_NATIVEXCOMPONENT_MOUSE_RELEASE) {
        LiSendMouseButtonEvent(BUTTON_ACTION_RELEASE, button);
    }
    return 0;
}

static napi_value MoonBridge_sendMouseMove(napi_env env, napi_callback_info info) {
    // LiSendMouseMoveEvent(deltaX, deltaY);
}
static napi_value MoonBridge_sendMousePosition(napi_env env, napi_callback_info info) {
    // LiSendMousePositionEvent(x, y, referenceWidth, referenceHeight);
}
static napi_value MoonBridge_sendMouseMoveAsMousePosition(napi_env env, napi_callback_info info) {
    // LiSendMouseMoveAsMousePositionEvent(deltaX, deltaY, referenceWidth, referenceHeight);
}
static napi_value MoonBridge_sendMouseButton(napi_env env, napi_callback_info info) {
    // LiSendMouseButtonEvent(buttonEvent, mouseButton);
}
static napi_value MoonBridge_sendMultiControllerInput(napi_env env, napi_callback_info info) {
    size_t argc = 9;
    napi_value args[9] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    int controllerNumber;
    int activeGamepadMask;
    int buttonFlags;
    int leftTriggerInt;
    int rightTriggerInt;
    int leftStickX ;
    int leftStickY;
    int rightStickX;
    int rightStickY;
    napi_get_value_int32(env, args[0], &controllerNumber);
    napi_get_value_int32(env, args[1], &activeGamepadMask);
    napi_get_value_int32(env, args[2], &buttonFlags);
    napi_get_value_int32(env, args[3], &leftTriggerInt);
    napi_get_value_int32(env, args[4], &rightTriggerInt);
    unsigned char leftTrigger = static_cast<unsigned char>(leftTriggerInt);
    unsigned char rightTrigger = static_cast<unsigned char>(rightTriggerInt);
    napi_get_value_int32(env, args[5], &leftStickX);
    napi_get_value_int32(env, args[6], &leftStickY);
    napi_get_value_int32(env, args[7], &rightStickX);
    napi_get_value_int32(env, args[8], &rightStickY);
   LiSendMultiControllerEvent(controllerNumber, activeGamepadMask, buttonFlags,
                               leftTrigger, rightTrigger, leftStickX, leftStickY, rightStickX, rightStickY);
    return nullptr;
}
static napi_value MoonBridge_sendTouchEvent(napi_env env, napi_callback_info info) {
    // LiSendTouchEvent(eventType, pointerId, x, y, pressureOrDistance,
    //                          contactAreaMajor, contactAreaMinor, rotation);
}
static napi_value MoonBridge_sendPenEvent(napi_env env, napi_callback_info info) {
    // LiSendPenEvent(eventType, toolType, penButtons, x, y, pressureOrDistance,
    //                       contactAreaMajor, contactAreaMinor, rotation, tilt);
}
static napi_value MoonBridge_sendControllerTouchEvent(napi_env env, napi_callback_info info) {
    // LiSendControllerArrivalEvent(controllerNumber, activeGamepadMask, type, supportedButtonFlags, capabilities);
}
static napi_value MoonBridge_sendControllerArrivalEvent(napi_env env, napi_callback_info info) {
    // LiSendControllerTouchEvent(controllerNumber, eventType, pointerId, x, y, pressure);
}
static napi_value MoonBridge_sendControllerMotionEvent(napi_env env, napi_callback_info info) {

    // LiSendControllerMotionEvent(controllerNumber, motionType, x, y, z);
}
static napi_value MoonBridge_sendControllerBatteryEvent(napi_env env, napi_callback_info info) {
    //  LiSendControllerBatteryEvent(controllerNumber, batteryState, batteryPercentage);
}
static napi_value MoonBridge_sendKeyboardInput(napi_env env, napi_callback_info info) {
    // LiSendKeyboardEvent2(keyCode, keyAction, modifiers, flags);
}
static napi_value MoonBridge_sendMouseHighResScroll(napi_env env, napi_callback_info info) {
    // LiSendHighResScrollEvent(scrollAmount);
}
static napi_value MoonBridge_sendMouseHighResHScroll(napi_env env, napi_callback_info info) {
    // LiSendHighResHScrollEvent(scrollAmount);
}
static napi_value MoonBridge_sendUtf8Text(napi_env env, napi_callback_info info) {
    // const char* utf8Text = (*env)->GetStringUTFChars(env, text, NULL);
    //   LiSendUtf8TextEvent(utf8Text, strlen(utf8Text));
    //  (*env)->ReleaseStringUTFChars(env, text, utf8Text);
}

static napi_value MoonBridge_findExternalAddressIP4(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    int stunPort;
    struct in_addr wanAddr;
    const char *stunHostNameStr = get_value_string(env, args[0]);
    int err = LiFindExternalAddressIP4(stunHostNameStr, stunPort, &wanAddr.s_addr);
    delete stunHostNameStr;

    if (err == 0) {
        char addrStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &wanAddr, addrStr, sizeof(addrStr));
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "moonlight-common-c", "Resolved WAN address to %{public}s", addrStr);
        napi_value result;
        napi_create_string_utf8(env, addrStr, sizeof(addrStr), &result);
        return result;
    } else {
        OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "moonlight-common-c", "STUN failed to get WAN address: %{public}d", err);
        return NULL;
    }
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
    napi_get_cb_info(env, info, nullptr, nullptr, &thisArg, &data);
    const char* avc = OH_AVCODEC_MIMETYPE_VIDEO_AVC;
   
    OH_AVCodec* m_decoder = OH_VideoDecoder_CreateByMime("video/hevc");
    //OH_AVCodec* m_decoder2 = OH_VideoDecoder_CreateByMime(OH_AVCODEC_MIMETYPE_VIDEO_AVC);
    napi_value global = nullptr;
    napi_get_global(env, &global);

    return thisArg;
}

static OH_NativeXComponent_Callback callback;

void MoonBridgeApi::Export(napi_env env, napi_value exports) {
    // 需要 api 9 没有真机测试
    // m_decoder = (IVideoDecoder *)new NativeVideoDecoder();
    // 软解码 ffmpeg cpu
    api->env = env;
    napi_property_descriptor descriptors[] = {
        {"startConnection", nullptr, MoonBridge_startConnection, nullptr, nullptr, nullptr, napi_default, nullptr},

        {"stopConnection", nullptr, MoonBridge_stopConnection, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"interruptConnection", nullptr, MoonBridge_interruptConnection, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"onVideoStatus", nullptr, MoonBridge_onVideoStatus, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"sendMultiControllerInput", nullptr, MoonBridge_sendMultiControllerInput, nullptr, nullptr, nullptr, napi_default, nullptr},

        {"onClStage", nullptr, On, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"onClStageFailed", nullptr, On, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"onClConnection", nullptr, On, nullptr, nullptr, nullptr, napi_default, nullptr},
    };

    napi_value result = nullptr;

    napi_define_class(env, "MoonBridgeNapi", NAPI_AUTO_LENGTH, MoonBridgeJavascriptClassConstructor, nullptr,
                      sizeof(descriptors) / sizeof(*descriptors), descriptors, &result);

    napi_set_named_property(env, exports, "MoonBridgeNapi", result);

    napi_value exportInstance;
    if (napi_get_named_property(env, exports, "__NATIVE_XCOMPONENT_OBJ__", &exportInstance) != napi_ok) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, 0, "PluginManager", "Export: napi_get_named_property fail");
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
napi_threadsafe_function MoonBridgeApi::getFunByName(char *eventName) {
    auto it = m_funRefs.find(eventName);
    if (it != m_funRefs.end()) {
        return it->second;
    }
    return nullptr;
}
int MoonBridgeApi::setFunByName(char *name, napi_threadsafe_function tsf) {
    napi_threadsafe_function d = getFunByName(name);
    if (d != nullptr) {
        napi_release_threadsafe_function(d, napi_tsfn_release);
    }
    m_funRefs[name] = tsf;
    return 0;
}
static void Napi_OnCallback(napi_env env, napi_value js_callback, void *context, void *data) {
    MoonBridgeCallBackInfo *info = static_cast<MoonBridgeCallBackInfo *>(data);

    napi_value params[1];
    napi_create_string_utf8(env, info->stage, NAPI_AUTO_LENGTH, &params[0]);
    napi_call_function(env, nullptr, js_callback, 1, params, nullptr);
}

napi_value MoonBridgeApi::Emit(char *eventName, void *value) {
    napi_threadsafe_function tsfn = api->getFunByName(eventName);
    if (tsfn != nullptr) {
        napi_call_threadsafe_function(tsfn, value, napi_tsfn_blocking);
    }
    return nullptr;
}
napi_value MoonBridgeApi::On(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value argv[2];
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    char *eventName = get_value_string(env, argv[0]);
    napi_value resourceName;
    napi_threadsafe_function tsfn;
    napi_create_string_latin1(env, eventName, NAPI_AUTO_LENGTH, &resourceName);
    napi_create_threadsafe_function(env, argv[1], NULL, resourceName, 0, 1, NULL, NULL, NULL, Napi_OnCallback, &tsfn);
    MoonBridgeApi::api->setFunByName(eventName, tsfn);
    return nullptr;
};