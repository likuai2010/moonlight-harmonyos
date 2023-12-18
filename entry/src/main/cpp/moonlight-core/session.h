//
// Created on 2023/12/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef moonlight_session_H
#define moonlight_session_H

#include "Limelight.h"
#include "video/decode.h"

class Session{
    explicit Session();
    virtual ~Session() {};
    static Session* get()
    {
        return s_ActiveSession;
    }
    private:
        static
        void clStageStarting(int stage);
    
        static
        void clStageFailed(int stage, int errorCode);
    
        static
        void clConnectionTerminated(int errorCode);
    
        static
        void clLogMessage(const char* format, ...);
    
        static
        void clRumble(unsigned short controllerNumber, unsigned short lowFreqMotor, unsigned short highFreqMotor);
    
        static
        void clConnectionStatusUpdate(int connectionStatus);
    
        static
        void clSetHdrMode(bool enabled);
    
        static
        void clRumbleTriggers(uint16_t controllerNumber, uint16_t leftTrigger, uint16_t rightTrigger);
    
        static
        void clSetMotionEventState(uint16_t controllerNumber, uint8_t motionType, uint16_t reportRateHz);
    
        static
        void clSetControllerLED(uint16_t controllerNumber, uint8_t r, uint8_t g, uint8_t b);
    
        static
        int arInit(int audioConfiguration,
                   const POPUS_MULTISTREAM_CONFIGURATION opusConfig,
                   void* arContext, int arFlags);
    
        static
        void arCleanup();
    
        static
        void arDecodeAndPlaySample(char* sampleData, int sampleLength);
    
        static
        int drSetup(int videoFormat, int width, int height, int frameRate, void*, int);
    
        static
        void drCleanup();
    
        static
        int drSubmitDecodeUnit(PDECODE_UNIT du);
    static Session* s_ActiveSession;
    
};



#endif //moonlight_MoonlightSession_H
