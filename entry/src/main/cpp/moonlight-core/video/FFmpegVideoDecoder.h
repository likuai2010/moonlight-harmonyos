//
// Created on 2023/12/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_VideoDecoder_H
#define moonlight_VideoDecoder_H
#include "decode.h"

// Disables the deblocking filter at the cost of image quality
#define DISABLE_LOOP_FILTER 0x1
// Uses the low latency decode flag (disables multithreading)
#define LOW_LATENCY_DECODE 0x2

extern "C" {
#include "libavcodec/avcodec.h"
}

class FFmpegVideoDecoder : public IVideoDecoder {
  public:
    explicit FFmpegVideoDecoder();
    ~FFmpegVideoDecoder();

    int setup(DECODER_PARAMETERS* params) override;
    void cleanup() override;
    int submitDecodeUnit(PDECODE_UNIT decode_unit) override;
    VIDEO_STATS* video_decode_stats() override;

  private:
    int decode(char* indata, int inlen);
    int m_stream_fps = 0;
    int m_frames_in = 0;
    int m_frames_out = 0;
    int m_frames_count = 0;
    int m_current_frame = 0, m_next_frame = 0;
    uint32_t m_last_frame = 0;
    VIDEO_STATS m_video_decode_stats = {};
    char* m_ffmpeg_buffer = nullptr;
    
    AVFrame* get_frame(bool native_frame);
    
    AVPacket* m_packet;
    AVBufferRef *hw_device_ctx = nullptr;
    const AVCodec* m_decoder = nullptr;
    AVCodecContext* m_decoder_context = nullptr;
    
    AVFrame** m_frames = nullptr;
    AVFrame *tmp_frame = nullptr;
    
    AVFrame* m_frame = nullptr;
};

#endif //moonlight_VideoDecoder_H
