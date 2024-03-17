//
// Created on 2023/12/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_NativeVideoDecoder_H
#define moonlight_NativeVideoDecoder_H
#include "decode.h"
#include <queue>
#include <string>
#include <thread>
#include <atomic>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avformat.h>


struct DataPacket{
    uint8_t *data;
        // 数据的大小（字节数）
    int      size;
    // 解码时间戳（DTS）
    int64_t  pts;
    // 演示时间戳（PTS）
    int64_t  dts;
    // 数据的持续时间
    int  duration;
    // 一些标志和辅助信息
    int   flags;
} ;

class VDecSignal {
public:
    std::mutex inMutex_;
    std::mutex outMutex_;
    std::condition_variable inCond_;
    std::condition_variable outCond_;
    std::queue<uint32_t> inQueue_;
    std::queue<uint32_t> outQueue_;
    std::queue<OH_AVMemory *> inBufferQueue_;
    std::queue<OH_AVMemory *> outBufferQueue_;
    std::queue<OH_AVCodecBufferAttr> attrQueue_;
    std::queue<DataPacket *> dataPacketQueue_;
};



class NativeVideoDecoder : public IVideoDecoder {
  public:
    explicit NativeVideoDecoder();
    ~NativeVideoDecoder();
    void start() override;
    void stop() override;
    int setup(DECODER_PARAMETERS params) override;
    void cleanup() override;
    int submitDecodeUnit(PDECODE_UNIT decode_unit) override;
    VIDEO_STATS *video_decode_stats() override;
    static bool supportedHW();
    DECODER_PARAMETERS *getParams() override;

private:
    void inputFunc();
    void outputFunc();
    int ExtractPacket();
    std::atomic<bool> m_is_running;
    VDecSignal *m_signal = nullptr;
    bool m_isFirst_frame = true;
    
    std::unique_ptr<std::thread> m_inputLoop = nullptr;
    std::unique_ptr<std::thread> m_outputLoop = nullptr;
    DataPacket *m_pkt = nullptr;
    
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
