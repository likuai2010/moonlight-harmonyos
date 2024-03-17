//
// Created on 2023/12/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "NativeVideoDecoder.h"
#include <bits/alltypes.h>
#include <multimedia/player_framework/native_averrors.h>
#include <stdarg.h>
#include <hilog/log.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <unistd.h>

#define DECODER_BUFFER_SIZE 92 * 1024 * 2
#define decodeLog(...) OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "NativeVideoDecoder",  __VA_ARGS__)

bool NativeVideoDecoder::supportedHW(){
   return OH_VideoDecoder_CreateByMime(OH_AVCODEC_MIMETYPE_VIDEO_AVC) != NULL;
}
NativeVideoDecoder::NativeVideoDecoder() {}
NativeVideoDecoder::~NativeVideoDecoder() {}

static void OnError(OH_AVCodec *codec, int32_t errorCode, void *userData) {
    (void)codec;
    (void)errorCode;
    (void)userData;
    decodeLog("Error received, errorCode: %{public}d", errorCode);
}
static void OnOutputFormatChanged(OH_AVCodec *codec, OH_AVFormat *format, void *userData) {
    (void)codec;
    (void)format;
    (void)userData;
    decodeLog("OnOutputFormatChanged received");
}

static void OnInputBufferAvailable(OH_AVCodec *codec, uint32_t index, OH_AVMemory *data, void *userData) {
    (void)codec;
    VDecSignal *signal_ = static_cast<VDecSignal *>(userData);
    std::unique_lock<std::mutex> lock(signal_->inMutex_);
    signal_->inQueue_.push(index);
    signal_->inBufferQueue_.push(data);
    signal_->inCond_.notify_all();
}

static void OnOutputBufferAvailable(OH_AVCodec *codec, uint32_t index, OH_AVMemory *data, OH_AVCodecBufferAttr *attr,
                                    void *userData) {
    (void)codec;
    VDecSignal *signal_ = static_cast<VDecSignal *>(userData);
    if (attr) {
        decodeLog("OnOutputBufferAvailable received, index: %{public}d, attr->size: %{public}d", index, attr->size);
        std::unique_lock<std::mutex> lock(signal_->outMutex_);
        signal_->outQueue_.push(index);
        signal_->outBufferQueue_.push(data);
        signal_->attrQueue_.push(*attr);
        // outFrameCount += attr->size > 0 ? 1 : 0;
        signal_->outCond_.notify_all();
    } else {
        decodeLog("OnOutputBufferAvailable error, attr is nullptr!");
    }
}
DECODER_PARAMETERS* NativeVideoDecoder::getParams() {
    return NULL;
}
int NativeVideoDecoder::setup(DECODER_PARAMETERS params) {
    m_stream_fps = params.frame_rate;
    decodeLog(
        "Setup with format: %{public}s, width: %{public}d, height: %{public}d, fps: %{public}d",
        params.video_format == VIDEO_FORMAT_H264 ? "H264" : "HEVC",
        params.width, params.height,
        params.frame_rate);
    switch (params.video_format) {
    case VIDEO_FORMAT_H264:
        decodeLog("  find decoder 264");
        m_decoder = OH_VideoDecoder_CreateByMime(OH_AVCODEC_MIMETYPE_VIDEO_AVC);
        break;
    case VIDEO_FORMAT_H265:
        decodeLog(" find decoder HEVC");
        m_decoder = OH_VideoDecoder_CreateByMime(OH_AVCODEC_MIMETYPE_VIDEO_AVC);
        break;
    }
    if (m_decoder == NULL) {
        decodeLog(" Couldn't find decoder");
        return -1;
    }

    m_signal = new VDecSignal();
    OH_AVFormat *format = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_WIDTH, params.width);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_HEIGHT, params.height);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_PIXEL_FORMAT, AV_PIXEL_FORMAT_NV21);
    // 配置解码器
    int ret = OH_VideoDecoder_Configure(m_decoder, format);
    OH_AVFormat_Destroy(format);
    OH_AVCodecAsyncCallback callback = {&OnError, &OnOutputFormatChanged, &OnInputBufferAvailable, &OnOutputBufferAvailable};
    OH_VideoDecoder_SetCallback(m_decoder, callback, m_signal);

    // 配置送显窗口参数
    // 从 XComponent 获取 window
    if (params.context != nullptr) {
        //OHNativeWindow *window = static_cast<OHNativeWindow *>(params.context);
        // 设置显示窗口
        //OH_VideoDecoder_SetSurface(m_decoder, window);
    } else {
        decodeLog(" Couldn't find set surface");
    }
    bool isSurfaceMode = true;
    // 配置 buffer info 信息
    OH_AVCodecBufferAttr info;
    m_ffmpeg_buffer = (char *)malloc(DECODER_BUFFER_SIZE + 64);
    if (m_ffmpeg_buffer == NULL) {
        decodeLog("Not enough memory");
        cleanup();
        return -1;
    }

    return DR_OK;
}

void NativeVideoDecoder::start() {
    m_is_running.store(true);
    m_inputLoop = std::make_unique<std::thread>(&NativeVideoDecoder::inputFunc, this);
    m_outputLoop = std::make_unique<std::thread>(&NativeVideoDecoder::outputFunc, this);

    OH_VideoDecoder_Start(m_decoder);
}
void NativeVideoDecoder::stop() {
    m_is_running.store(false);
    if (m_inputLoop != nullptr && m_inputLoop->joinable()) {
        std::unique_lock<std::mutex> lock(m_signal->inMutex_);
        m_signal->inCond_.notify_all();
        lock.unlock();
        m_inputLoop->join();
    }
    if (m_outputLoop != nullptr && m_outputLoop->joinable()) {
        std::unique_lock<std::mutex> lock(m_signal->outMutex_);
        m_signal->outCond_.notify_all();
        lock.unlock();
        m_outputLoop->join();
    }
    decodeLog("start stop!");
    OH_VideoDecoder_Stop(m_decoder);
}
void flush() {
    // OH_VideoDecoder_Flush(m_decoder);
}
void NativeVideoDecoder::cleanup() {
    OH_VideoDecoder_Destroy(m_decoder);
}
VIDEO_STATS *NativeVideoDecoder::video_decode_stats() {
    return &m_video_decode_stats;
}

int NativeVideoDecoder::ExtractPacket() {
    if (m_signal->dataPacketQueue_.empty()){
        return -1;
    }
    m_pkt = m_signal->dataPacketQueue_.front();
    m_signal->dataPacketQueue_.pop();
    return 0;
}
// 解码现成
void NativeVideoDecoder::inputFunc() {
    while (true) {
        if (!m_is_running.load()) {
            break;
        }
        std::unique_lock<std::mutex> lock(m_signal->inMutex_);
        m_signal->inCond_.wait(lock, [this]() { return (m_signal->inQueue_.size() > 0 || !m_is_running.load()); });

        if (!m_is_running.load()) {
            break;
        }
        if (m_signal->inBufferQueue_.empty())
            continue;
        uint32_t index = m_signal->inQueue_.front();
        auto buffer = m_signal->inBufferQueue_.front();
        lock.unlock();
        if ((ExtractPacket() != AV_ERR_OK)) {
            continue;
        }
        OH_AVCodecBufferAttr info;
        info.size = m_pkt.size;
        info.offset = 0;
        info.pts = m_pkt.pts;

        if (buffer == nullptr) {
            decodeLog("Fatal: GetInputBuffer fail");
        }
        uint8_t* dd = OH_AVMemory_GetAddr(buffer);
        int size = OH_AVMemory_GetSize(buffer);
        if (size >= m_pkt.size) { 
            memcpy(dd, m_pkt.data, m_pkt.size);
        }
        else{
            decodeLog("Fatal: big data %{public}d >= %{public}d", size, m_pkt.size);
        }
        
        int32_t ret = 0;
        if (m_isFirst_frame) {
            info.flags = AVCODEC_BUFFER_FLAGS_NONE;
            ret = OH_VideoDecoder_PushInputData(m_decoder, index, info);
            m_isFirst_frame = false;
        } else {
            info.flags = AVCODEC_BUFFER_FLAGS_NONE;
            ret = OH_VideoDecoder_PushInputData(m_decoder, index, info);
        }

        if (ret != AV_ERR_OK) {
            decodeLog("Fatal error, exit");
            break;
        }

        // timeStamp_ += FRAME_DURATION_US;
        m_signal->inQueue_.pop();
        m_signal->inBufferQueue_.pop();
    }
}

void NativeVideoDecoder::outputFunc() {
    while (true) {
        if (!m_is_running.load()) {
            decodeLog("stop, exit");
            break;
        }

        std::unique_lock<std::mutex> lock(m_signal->outMutex_);
        m_signal->outCond_.wait(lock, [this]() { return (m_signal->outQueue_.size() > 0 || !m_is_running.load()); });

        if (!m_is_running.load()) {
            decodeLog("wait to stop, exit");
            break;
        }
      
        uint32_t index = m_signal->outQueue_.front();
        OH_AVCodecBufferAttr attr = m_signal->attrQueue_.front();
        OH_AVMemory *data = m_signal->outBufferQueue_.front();
        decodeLog("outputFunc rite frame: ${public}d");

        if (attr.flags == AVCODEC_BUFFER_FLAGS_EOS) {
            // outFrameCount
            decodeLog("decode eos, write frame: ${public}d");
            m_is_running.store(false);
        }
        // 显示
        if (OH_VideoDecoder_RenderOutputData(m_decoder, index) != AV_ERR_OK) {
            decodeLog("Fatal: RenderOutputData fail");
            break;
        }
        m_signal->outBufferQueue_.pop();
        m_signal->attrQueue_.pop();
        m_signal->outQueue_.pop();
    }
}

int NativeVideoDecoder::submitDecodeUnit(PDECODE_UNIT du) {

    if (m_frames_in == 0 && du->frameType != FRAME_TYPE_IDR) {
        return DR_NEED_IDR;
    }
    if (du->fullLength < DECODER_BUFFER_SIZE) {

        PLENTRY entry = du->bufferList;
        if (!m_last_frame) {
            m_video_decode_stats.measurementStartTimestamp = LiGetMillis();
            m_last_frame = du->frameNumber;
        } else {
            // Any frame number greater than m_LastFrameNumber + 1 represents a
            // dropped frame
            m_video_decode_stats.networkDroppedFrames +=
                du->frameNumber - (m_last_frame + 1);
            m_video_decode_stats.totalFrames +=
                du->frameNumber - (m_last_frame + 1);
            m_last_frame = du->frameNumber;
        }

        m_video_decode_stats.receivedFrames++;
        m_video_decode_stats.totalFrames++;

        int length = 0;
        while (entry != NULL) {
            if (length > DECODER_BUFFER_SIZE) {
                decodeLog("FFmpeg: Big buffer to decode... !");
            }
            memcpy(m_ffmpeg_buffer + length, entry->data, entry->length);
            length += entry->length;
            entry = entry->next;
        }

        m_video_decode_stats.totalReassemblyTime +=
            LiGetMillis() - du->receiveTimeMs;

        m_frames_in++;

        uint64_t before_decode = LiGetMillis();

        if (length > DECODER_BUFFER_SIZE) {
            decodeLog("FFmpeg: Big buffer to decode...");
        }

        DataPacket pkt =  DataPacket{
        };
        pkt.data = (uint8_t *)m_ffmpeg_buffer;
        pkt.size = length;
        if (du->frameType == FRAME_TYPE_IDR) {
            pkt.flags = AVCODEC_BUFFER_FLAGS_INCOMPLETE_FRAME;
        } else {
            pkt.flags = 0;
        }
        m_signal->dataPacketQueue_.push(&pkt);
        m_frames_out++;
        m_video_decode_stats.totalDecodeTime +=
            LiGetMillis() - before_decode;

        // Also count the frame-to-frame delay if the decoder is delaying
        // frames until a subsequent frame is submitted.
        m_video_decode_stats.totalDecodeTime +=
            (m_frames_in - m_frames_out) * (1000 / m_stream_fps);
        m_video_decode_stats.decodedFrames++;
        // m_frame = get_frame(true);
        //AVFrameHolder::instance().push(m_frame);
    } else {
        decodeLog("FFmpeg: Big buffer to decode... 2");
    }
    return DR_OK;
}
