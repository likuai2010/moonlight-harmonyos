//
// Created on 2023/12/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "NativeVideoDecoder.h"
#include <linux/dvb/video.h>
#include <stdarg.h>
#include <hilog/log.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>

#define DECODER_BUFFER_SIZE 92 * 1024 * 2

void decodeLog(const char *format, ...) {
    va_list va;
    va_start(va, format);
    OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, "NativeVideoDecoder", format, va);
    va_end(va);
}

NativeVideoDecoder::NativeVideoDecoder() {}
NativeVideoDecoder::~NativeVideoDecoder() {}

int NativeVideoDecoder::setup(DECODER_PARAMETERS *params) {
    m_stream_fps = params->frame_rate;
    decodeLog(
        "Setup with format: {}, width: {}, height: {}, fps: {}",
        params->video_format == VIDEO_FORMAT_H264 ? "H264" : "HEVC",
        params->width, params->height,
        params->frame_rate);
    switch (params->video_format) {
    case VIDEO_FORMAT_H264:
        m_decoder = OH_VideoDecoder_CreateByMime(OH_AVCODEC_MIMETYPE_VIDEO_AVC);
        break;
    case VIDEO_FORMAT_H265:
        decodeLog(" Couldn't find decoder h256");
        m_decoder = OH_VideoDecoder_CreateByMime(OH_AVCODEC_MIMETYPE_VIDEO_AVC);
        break;
    }
    if (m_decoder == NULL) {
        decodeLog(" Couldn't find decoder");
        return -1;
    }
    OH_AVFormat *format = OH_AVFormat_Create();
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_WIDTH, params->width);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_HEIGHT, params->height);
    OH_AVFormat_SetIntValue(format, OH_MD_KEY_PIXEL_FORMAT, AV_PIXEL_FORMAT_NV21);
    // 配置解码器
    int dd = OH_VideoDecoder_Configure(m_decoder, format);
    OH_AVFormat_Destroy(format);
    OH_AVCodecAsyncCallback callback = {
        .onNeedInputData = {
        
        }
    };
    OH_VideoDecoder_SetCallback(m_decoder, callback, nullptr);

    // 配置送显窗口参数
    // int32_t ret = OH_VideoDecoder_SetSurface(m_decoder, window);    // 从 XComponent 获取 window
    bool isSurfaceMode = true;
    // 配置 buffer info 信息
    OH_AVCodecBufferAttr info;

    // 开始解码
    return DR_OK;
}
void NativeVideoDecoder::cleanup() {
}
VIDEO_STATS *NativeVideoDecoder::video_decode_stats() {
    return nullptr;
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
        //    if (du->frameType == FRAME_TYPE_IDR) {
        //        m_Pkt->flags = AV_PKT_FLAG_KEY;
        //    }
        //    else {
        //        m_Pkt->flags = 0;
        //    }
        // 配置 buffer info 信息
        OH_AVCodecBufferAttr *m_packet;
        m_packet->offset = 0;
        //m_packet->pts = &m_ffmpeg_buffer;
        m_packet->size = length;
        OH_AVErrCode err = OH_VideoDecoder_PushInputData(m_decoder, 0, *m_packet);
        if (err != 0) {
            char error[512];
            //av_strerror(err, error, sizeof(error));
            decodeLog("FFmpeg: Decode failed - %{public}s", error);
        } else if (err == 0) {
            m_frames_out++;
            m_video_decode_stats.totalDecodeTime +=
                LiGetMillis() - before_decode;

            // Also count the frame-to-frame delay if the decoder is delaying
            // frames until a subsequent frame is submitted.
            m_video_decode_stats.totalDecodeTime +=
                (m_frames_in - m_frames_out) * (1000 / m_stream_fps);
            m_video_decode_stats.decodedFrames++;
            // m_frame = get_frame(true);
            //  AVFrameHolder::instance().push(m_frame);
        }
    } else {
        decodeLog("FFmpeg: Big buffer to decode... 2");
    }
    return DR_OK;
}
