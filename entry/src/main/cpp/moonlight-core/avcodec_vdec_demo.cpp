//
// Created on 2024/3/17.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "avcodec_vdec_demo.h"
#include <iostream>
#include <unistd.h>

using namespace OHOS;
using namespace OHOS::Media;
using namespace std;
namespace {
    constexpr uint32_t DEFAULT_WIDTH = 480;
    constexpr uint32_t DEFAULT_HEIGHT = 360;
    constexpr uint32_t DEFAULT_FRAME_RATE = 30;
    constexpr uint32_t MAX_INPUT_BUFFER_SIZE = 30000;
    constexpr uint32_t FRAME_DURATION_US = 33000;
    constexpr uint32_t DEFAULT_FRAME_COUNT = 1;
} // namespace

void VDecDemo::RunCase() {
    DEMO_CHECK_AND_RETURN_LOG(CreateVdec() == MSERR_OK, "Fatal: CreateVdec fail");
    format.PutIntValue("width", DEFAULT_WIDTH);
    format.PutIntValue("height", DEFAULT_HEIGHT);
    format.PutIntValue("pixel_format", NV21);
    format.PutIntValue("frame_rate", DEFAULT_FRAME_RATE);
    format.PutIntValue("max_input_size", MAX_INPUT_BUFFER_SIZE);
    DEMO_CHECK_AND_RETURN_LOG(Configure(format) == MSERR_OK, "Fatal: Configure fail");

    DEMO_CHECK_AND_RETURN_LOG(SetSurface() == 0, "Fatal: SetSurface fail");
    DEMO_CHECK_AND_RETURN_LOG(Prepare() == 0, "Fatal: Prepare fail");
    DEMO_CHECK_AND_RETURN_LOG(Start() == 0, "Fatal: Start fail");
    sleep(3); // start run 3s
    DEMO_CHECK_AND_RETURN_LOG(Stop() == 0, "Fatal: Stop fail");
    DEMO_CHECK_AND_RETURN_LOG(Release() == 0, "Fatal: Release fail");
}

int32_t VDecDemo::CreateVdec() {
    vdec_ = VideoDecoderFactory::CreateByMime("video/avc");
    DEMO_CHECK_AND_RETURN_RET_LOG(vdec_ != nullptr, MSERR_UNKNOWN, "Fatal: CreateByMime fail");

    signal_ = make_shared<VDecSignal>();
    DEMO_CHECK_AND_RETURN_RET_LOG(signal_ != nullptr, MSERR_UNKNOWN, "Fatal: No memory");

    cb_ = make_unique<VDecDemoCallback>(signal_);
    DEMO_CHECK_AND_RETURN_RET_LOG(cb_ != nullptr, MSERR_UNKNOWN, "Fatal: No memory");
    DEMO_CHECK_AND_RETURN_RET_LOG(vdec_->SetCallback(cb_) == MSERR_OK, MSERR_UNKNOWN, "Fatal: SetCallback fail");

    return MSERR_OK;
}

int32_t VDecDemo::Configure(const Format &format) { return vdec_->Configure(format); }

int32_t VDecDemo::Prepare() { return vdec_->Prepare(); }

int32_t VDecDemo::Start() {
    isRunning_.store(true);

    testFile_ = std::make_unique<std::ifstream>();
    DEMO_CHECK_AND_RETURN_RET_LOG(testFile_ != nullptr, MSERR_UNKNOWN, "Fatal: No memory");
    testFile_->open("/data/media/video.es", std::ios::in | std::ios::binary);

    inputLoop_ = make_unique<thread>(&VDecDemo::InputFunc, this);
    DEMO_CHECK_AND_RETURN_RET_LOG(inputLoop_ != nullptr, MSERR_UNKNOWN, "Fatal: No memory");

    outputLoop_ = make_unique<thread>(&VDecDemo::OutputFunc, this);
    DEMO_CHECK_AND_RETURN_RET_LOG(outputLoop_ != nullptr, MSERR_UNKNOWN, "Fatal: No memory");

    return vdec_->Start();
}

int32_t VDecDemo::Stop() {
    isRunning_.store(false);

    if (inputLoop_ != nullptr && inputLoop_->joinable()) {
        unique_lock<mutex> lock(signal_->inMutex_);
        signal_->inQueue_.push(0);
        signal_->inCond_.notify_all();
        lock.unlock();
        inputLoop_->join();
        inputLoop_.reset();
    }

    if (outputLoop_ != nullptr && outputLoop_->joinable()) {
        unique_lock<mutex> lock(signal_->outMutex_);
        signal_->outQueue_.push(0);
        signal_->outCond_.notify_all();
        lock.unlock();
        outputLoop_->join();
        outputLoop_.reset();
    }

    return vdec_->Stop();
}

int32_t VDecDemo::Flush() { return vdec_->Flush(); }

int32_t VDecDemo::Reset() { return vdec_->Reset(); }

int32_t VDecDemo::Release() { return vdec_->Release(); }

int32_t VDecDemo::SetSurface() {
    sptr<Rosen::WindowOption> option = new Rosen::WindowOption();
    option->SetWindowRect({0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT});
    option->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_LAUNCHING);
    option->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
    sptr<Rosen::Window> window = Rosen::Window::Create("avcodec video decoder window", option);
    DEMO_CHECK_AND_RETURN_RET_LOG(window != nullptr && window->GetSurfaceNode() != nullptr, MSERR_UNKNOWN, "Fatal");

    sptr<Surface> surface = window->GetSurfaceNode()->GetSurface();
    window->Show();
    DEMO_CHECK_AND_RETURN_RET_LOG(surface != nullptr, MSERR_UNKNOWN, "Fatal: get surface fail");
    return vdec_->SetOutputSurface(surface);
}

void VDecDemo::InputFunc() {
    while (true) {
        if (!isRunning_.load()) {
            break;
        }

        unique_lock<mutex> lock(signal_->inMutex_);
        signal_->inCond_.wait(lock, [this]() { return signal_->inQueue_.size() > 0; });

        if (!isRunning_.load()) {
            break;
        }

        uint32_t index = signal_->inQueue_.front();
        auto buffer = vdec_->GetInputBuffer(index);
        DEMO_CHECK_AND_BREAK_LOG(buffer != nullptr, "Fatal: GetInputBuffer fail");
        DEMO_CHECK_AND_BREAK_LOG(testFile_ != nullptr && testFile_->is_open(), "Fatal: open file fail");

        constexpr uint32_t bufferSize = 0; // replace with the actual size
        char *fileBuffer = static_cast<char *>(malloc(sizeof(char) * bufferSize + 1));
        DEMO_CHECK_AND_BREAK_LOG(fileBuffer != nullptr, "Fatal: malloc fail");

        (void)testFile_->read(fileBuffer, bufferSize);
        if (memcpy_s(buffer->GetBase(), buffer->GetSize(), fileBuffer, bufferSize) != EOK) {
            free(fileBuffer);
            cout << "Fatal: memcpy fail" << endl;
            break;
        }

        AVCodecBufferInfo info;
        info.size = bufferSize;
        info.offset = 0;
        info.presentationTimeUs = timeStamp_;

        int32_t ret = MSERR_OK;
        if (isFirstFrame_) {
            ret = vdec_->QueueInputBuffer(index, info, AVCODEC_BUFFER_FLAG_CODEC_DATA);
            isFirstFrame_ = false;
        } else {
            ret = vdec_->QueueInputBuffer(index, info, AVCODEC_BUFFER_FLAG_NONE);
        }

        free(fileBuffer);
        timeStamp_ += FRAME_DURATION_US;
        signal_->inQueue_.pop();

        frameCount_++;
        if (frameCount_ == DEFAULT_FRAME_COUNT) {
            cout << "Finish decode, exit" << endl;
            break;
        }

        if (ret != MSERR_OK) {
            cout << "Fatal error, exit" << endl;
            break;
        }
    }
}

void VDecDemo::OutputFunc() {
    while (true) {
        if (!isRunning_.load()) {
            break;
        }

        unique_lock<mutex> lock(signal_->outMutex_);
        signal_->outCond_.wait(lock, [this]() { return signal_->outQueue_.size() > 0; });

        if (!isRunning_.load()) {
            break;
        }

        uint32_t index = signal_->outQueue_.front();
        if (vdec_->ReleaseOutputBuffer(index, true) != MSERR_OK) {
            cout << "Fatal: ReleaseOutputBuffer fail" << endl;
            break;
        }

        signal_->outQueue_.pop();
    }
}

VDecDemoCallback::VDecDemoCallback(shared_ptr<VDecSignal> signal) : signal_(signal) {}

void VDecDemoCallback::OnError(AVCodecErrorType errorType, int32_t errorCode) {
    cout << "Error received, errorType:" << errorType << " errorCode:" << errorCode << endl;
}

void VDecDemoCallback::OnOutputFormatChanged(const Format &format) { cout << "OnOutputFormatChanged received" << endl; }

void VDecDemoCallback::OnInputBufferAvailable(uint32_t index) {
    cout << "OnInputBufferAvailable received, index:" << index << endl;
    unique_lock<mutex> lock(signal_->inMutex_);
    signal_->inQueue_.push(index);
    signal_->inCond_.notify_all();
}

void VDecDemoCallback::OnOutputBufferAvailable(uint32_t index, AVCodecBufferInfo info, AVCodecBufferFlag flag) {
    cout << "OnOutputBufferAvailable received, index:" << index << endl;
    unique_lock<mutex> lock(signal_->outMutex_);
    signal_->outQueue_.push(index);
    signal_->outCond_.notify_all();
}