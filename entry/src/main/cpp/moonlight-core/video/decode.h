//
// Created on 2023/12/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".


#include <Limelight.h>

#define SDL_CODE_FRAME_READY 0
#define MAX_SLICES 4
#include <utility>
#include <stdlib.h>

using Size = std::pair<int, int>;

typedef struct _VIDEO_STATS {
    uint32_t receivedFrames;
    uint32_t decodedFrames;
    uint32_t renderedFrames;
    uint32_t totalFrames;
    uint32_t networkDroppedFrames;
    uint32_t totalReassemblyTime;
    uint32_t totalDecodeTime;
    uint32_t totalRenderTime;
    float totalFps;
    float receivedFps;
    float decodedFps;
    float renderedFps;
    uint32_t measurementStartTimestamp;
} VIDEO_STATS, *PVIDEO_STATS;
enum VideoDecoderSelection
{
    VDS_AUTO,
    VDS_FORCE_HARDWARE,
    VDS_FORCE_SOFTWARE
};

typedef struct _DECODER_PARAMETERS {
    void* context;
    int video_format;
    int width;
    int height;
    int frame_rate;
    int dr_flags;
} DECODER_PARAMETERS;

class IVideoDecoder {
public:
    virtual ~IVideoDecoder() {}
    virtual int setup(DECODER_PARAMETERS* params) = 0;
    virtual void start(){};
    virtual void stop(){};
    virtual void cleanup() = 0;
    
    virtual int submitDecodeUnit(PDECODE_UNIT du) = 0;
    virtual VIDEO_STATS* video_decode_stats() = 0;
    // TODO hdr
    //virtual bool isHdrSupported() = 0;
    //virtual void setHdrMode(bool enabled) = 0;
};



