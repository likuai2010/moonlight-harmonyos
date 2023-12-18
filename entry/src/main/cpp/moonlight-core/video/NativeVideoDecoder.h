//
// Created on 2023/12/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_NativeVideoDecoder_H
#define moonlight_NativeVideoDecoder_H
#include "decode.h"

#include <linux/bcache.h>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avformat.h>

class NativeVideoDecoder : IVideoDecoder {
  public:
    explicit NativeVideoDecoder();
    ~NativeVideoDecoder();
    int setup(DECODER_PARAMETERS *params) override;
    void cleanup() override;
    int submitDecodeUnit(PDECODE_UNIT decode_unit) override;
    VIDEO_STATS *video_decode_stats() override;

  private:
    int m_stream_fps = 0;
    int m_frames_in = 0;
    int m_frames_out = 0;
    int m_frames_count = 0;
    int m_current_frame = 0, m_next_frame = 0;
    uint32_t m_last_frame = 0;
    VIDEO_STATS m_video_decode_stats = {};
    char *m_ffmpeg_buffer = nullptr;
    
    OH_AVCodec* m_decoder = nullptr;
};

#endif // moonlight_NativeVideoDecoder_H
