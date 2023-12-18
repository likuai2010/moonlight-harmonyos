//
// Created on 2023/12/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".
#include <Limelight.h>
#include "FFmpegVideoDecoder.h"
#include <hilog/log.h>
#include <stdio.h>

extern "C" {
#include <libavcodec/avcodec.h>
}


#define DECODER_BUFFER_SIZE 92 * 1024 * 2


FFmpegVideoDecoder::FFmpegVideoDecoder() {}
FFmpegVideoDecoder::~FFmpegVideoDecoder() {}

int FFmpegVideoDecoder::setup(DECODER_PARAMETERS* params) {

//
//    m_stream_fps = params->frame_rate;
//    decodeLog(
//        "FFmpeg: Setup with format: {}, width: {}, height: {}, fps: {}",
//        params->video_format == VIDEO_FORMAT_H264 ? "H264" : "HEVC",
//        params->width, params->height,
//        params->frame_rate);
//    av_log_set_level(AV_LOG_DEBUG);
//    // av_log_set_callback(&ffmpegLog);
//
//#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(58, 10, 100)
//    avcodec_register_all();
//#endif
//    m_packet = av_packet_alloc();
//    int perf_lvl = LOW_LATENCY_DECODE;
//    switch (params->video_format) {
//    case VIDEO_FORMAT_H264:
      m_decoder = avcodec_find_decoder_by_name("h264");
//        break;
//    case VIDEO_FORMAT_H265:
//        m_decoder = avcodec_find_decoder_by_name("hevc");
//        break;
//    }
//    if (m_decoder == NULL) {
//        decodeLog("FFmpeg: Couldn't find decoder");
//        return -1;
//    }
//    m_decoder_context = avcodec_alloc_context3(m_decoder);
//    if (m_decoder_context == NULL) {
//        decodeLog("FFmpeg: Couldn't allocate context");
//        return -1;
//    }
//    m_decoder_context->width = params->width;
//    m_decoder_context->height = params->height;
//    m_decoder_context->pix_fmt = AV_PIX_FMT_VIDEOTOOLBOX;
//    int err = avcodec_open2(m_decoder_context, m_decoder, NULL);
//    if (err < 0) {
//        decodeLog("FFmpeg: Couldn't open codec");
//        return err;
//    }
//    m_frames_count = 2;
//    m_frames = (AVFrame **)malloc(m_frames_count * sizeof(AVFrame *));
//    if (m_frames == NULL) {
//        decodeLog("FFmpeg: Couldn't allocate frames");
//        return -1;
//    }
//    tmp_frame = av_frame_alloc();
//    for (int i = 0; i < m_frames_count; i++) {
//        m_frames[i] = av_frame_alloc();
//        if (m_frames[i] == NULL) {
//            decodeLog("FFmpeg: Couldn't allocate frame");
//            return -1;
//        }
//
//        //        m_frames[i]->format = AV_PIX_FMT_VIDEOTOOLBOX;
//        m_frames[i]->format = AV_PIX_FMT_YUV420P;
//        m_frames[i]->width = params->width;
//        m_frames[i]->height = params->height;
//
//        int err = av_frame_get_buffer(m_frames[i], 256);
//        if (err < 0) {
//            decodeLog("FFmpeg: Couldn't allocate frame buffer:");
//            return -1;
//        }
//    }
//    if (perf_lvl & DISABLE_LOOP_FILTER)
//        // Skip the loop filter for performance reasons
//        m_decoder_context->skip_loop_filter = AVDISCARD_ALL;
//
//    if (perf_lvl & LOW_LATENCY_DECODE)
//        // Use low delay single threaded encoding
//        m_decoder_context->flags |= AV_CODEC_FLAG_LOW_DELAY;
//    m_ffmpeg_buffer =
//        (char *)malloc(DECODER_BUFFER_SIZE + AV_INPUT_BUFFER_PADDING_SIZE);
//    if (m_ffmpeg_buffer == NULL) {
//        decodeLog("FFmpeg: Not enough memory");
//        cleanup();
//        return -1;
//    }
//    decodeLog("FFmpeg: Setup done!");
    return DR_OK;
}

void FFmpegVideoDecoder::cleanup() {
//    decodeLog("FFmpeg: Cleanup...");
//
    av_packet_free(&m_packet);
//
//    if (hw_device_ctx) {
//        av_buffer_unref(&hw_device_ctx);
//    }
//
//    if (m_decoder_context) {
//        avcodec_close(m_decoder_context);
//        av_free(m_decoder_context);
//        m_decoder_context = NULL;
//    }
//
//    if (m_frames) {
//        for (int i = 0; i < m_frames_count; i++) {
//            if (m_frames[i])
//                av_frame_free(&m_frames[i]);
//        }
//
//        free(m_frames);
//        m_frames = nullptr;
//    }
//
//    if (tmp_frame) {
//        av_frame_free(&tmp_frame);
//    }
//
//    if (m_ffmpeg_buffer) {
//        free(m_ffmpeg_buffer);
//        m_ffmpeg_buffer = nullptr;
//    }
//
//    // AVFrameHolder::instance().cleanup();
//
//    decodeLog("FFmpeg: Cleanup done!");
}

int FFmpegVideoDecoder::submitDecodeUnit(PDECODE_UNIT du) {
    // If this is the first frame, reject anything that's not an IDR frame
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
                //decodeLog("FFmpeg: Big buffer to decode... !");
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
           // decodeLog("FFmpeg: Big buffer to decode...");
        }
        //    if (du->frameType == FRAME_TYPE_IDR) {
        //        m_Pkt->flags = AV_PKT_FLAG_KEY;
        //    }
        //    else {
        //        m_Pkt->flags = 0;
        //    }
        if (decode(m_ffmpeg_buffer, length) == 0) {
            m_frames_out++;
            m_video_decode_stats.totalDecodeTime +=
                LiGetMillis() - before_decode;

            // Also count the frame-to-frame delay if the decoder is delaying
            // frames until a subsequent frame is submitted.
            m_video_decode_stats.totalDecodeTime +=
                (m_frames_in - m_frames_out) * (1000 / m_stream_fps);
            m_video_decode_stats.decodedFrames++;

            m_frame = get_frame(true);
            // AVFrameHolder::instance().push(m_frame);
        }
    } else {
        //decodeLog("FFmpeg: Big buffer to decode... 2");
    }
    return DR_OK;
}

int FFmpegVideoDecoder::decode(char *indata, int inlen) {
    m_packet->data = (uint8_t *)indata;
    m_packet->size = inlen;

//    int err = avcodec_send_packet(m_decoder_context, m_packet);
//
//    if (err != 0) {
//        char error[512];
//        //av_strerror(err, error, sizeof(error));
//        decodeLog("FFmpeg: Decode failed - %{public}s", error);
//    }
    //return err != 0 ? err : 0;
    return DR_OK;
}

AVFrame *FFmpegVideoDecoder::get_frame(bool native_frame) {
//    int err = avcodec_receive_frame(m_decoder_context, tmp_frame);
//
//    if (hw_device_ctx) {
//        if ((err = av_hwframe_transfer_data(m_frames[m_next_frame], tmp_frame, 0)) < 0) {
//            decodeLog("FFmpeg: Error transferring the data to system memory with error {}", err);
//            return NULL;
//        }
//        av_frame_copy_props(m_frames[m_next_frame], tmp_frame);
//        //        m_frames[m_next_frame] = sw_frame;
//    } else {
//        m_frames[m_next_frame] = tmp_frame;
//    }
//
//    if (err == 0) {
//        m_current_frame = m_next_frame;
//        m_next_frame = (m_current_frame + 1) % m_frames_count;
//        if (/*ffmpeg_decoder == SOFTWARE ||*/ native_frame)
//            return m_frames[m_current_frame];
//    } else if (err != AVERROR(EAGAIN)) {
//        char error[512];
//        av_strerror(err, error, sizeof(error));
//        decodeLog("FFmpeg: Receive failed - %d/%s", err, error);
//    }
    return NULL;
}
VIDEO_STATS *FFmpegVideoDecoder::video_decode_stats() {
    uint64_t now = LiGetMillis();
    m_video_decode_stats.totalFps =
        (float)m_video_decode_stats.totalFrames /
        ((float)(now - m_video_decode_stats.measurementStartTimestamp) /
         1000);
    m_video_decode_stats.receivedFps =
        (float)m_video_decode_stats.receivedFrames /
        ((float)(now - m_video_decode_stats.measurementStartTimestamp) /
         1000);
    m_video_decode_stats.decodedFps =
        (float)m_video_decode_stats.decodedFrames /
        ((float)(now - m_video_decode_stats.measurementStartTimestamp) /
         1000);
    return (VIDEO_STATS *)&m_video_decode_stats;
}
