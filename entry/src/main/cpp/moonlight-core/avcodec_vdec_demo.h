//
// Created on 2024/3/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef AVCODEC_VDEC_DEMO_H
#define AVCODEC_VDEC_DEMO_H

#include <atomic>
#include <fstream>
#include <thread>
#include <queue>
#include <string>
#include <multimedia/player_framework/native_avcodec_videodecoder.h>
#include <multimedia/player_framework/native_avcodec_base.h>
#include <multimedia/player_framework/native_avformat.h>

namespace OHOS {
    namespace Media {
        class VDecSignal {
        public:
            std::mutex inMutex_;
            std::mutex outMutex_;
            std::condition_variable inCond_;
            std::condition_variable outCond_;
            std::queue<uint32_t> inQueue_;
            std::queue<uint32_t> outQueue_;
        };

        class VDecDemoCallback : public AVCodecCallback {
        public:
            explicit VDecDemoCallback(std::shared_ptr<VDecSignal> signal);
            virtual ~VDecDemoCallback() = default;

            void OnError(AVCodecErrorType errorType, int32_t errorCode) override;
            void OnOutputFormatChanged(const Format &format) override;
            void OnInputBufferAvailable(uint32_t index) override;
            void OnOutputBufferAvailable(uint32_t index, AVCodecBufferInfo info, AVCodecBufferFlag flag) override;

        private:
            std::shared_ptr<VDecSignal> signal_;
        };

        class VDecDemo : {
        public:
            VDecDemo() = default;
            virtual ~VDecDemo() = default;
            void RunCase();

        private:
            int32_t CreateVdec();
            int32_t Configure(const Format &format);
            int32_t Prepare();
            int32_t Start();
            int32_t Stop();
            int32_t Flush();
            int32_t Reset();
            int32_t Release();
            int32_t SetSurface();
            void InputFunc();
            void OutputFunc();

            std::atomic<bool> isRunning_ = false;
            std::unique_ptr<std::ifstream> testFile_;
            std::unique_ptr<std::thread> inputLoop_;
            std::unique_ptr<std::thread> outputLoop_;
            std::shared_ptr<AVCodecVideoDecoder> vdec_;
            std::shared_ptr<VDecSignal> signal_;
            std::shared_ptr<VDecDemoCallback> cb_;
            bool isFirstFrame_ = true;
            int64_t timeStamp_ = 0;
            uint32_t frameCount_ = 0;
        };
    } // namespace Media
} // namespace OHOS
#endif // AVCODEC_VDEC_DEMO_H