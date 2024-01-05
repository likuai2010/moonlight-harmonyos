# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(SDL3 LANGUAGES C CXX VERSION "3.0.0")
set(SDL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sdl-ohos)

message("sdl mu ${SDL_DIR}")
include_directories(${SDL_DIR}/include)
include_directories(${SDL_DIR}/src)

add_compile_definitions(OHOS __OHOS__)
# build.config
# include/build_config/SDL_build_config_ohos.h

# base lib
file(GLOB base ${SDL_DIR}/src/*.c)
file(GLOB timer ${SDL_DIR}/src/timer/*.c ${SDL_DIR}/src/timer/unix/SDL_systimer.c)
file(GLOB stdlib ${SDL_DIR}/src/stdlib/*.c)
file(GLOB thread  ${SDL_DIR}/src/thread/SDL_thread.c ${SDL_DIR}/src/thread/pthread/*.c)
file(GLOB events ${SDL_DIR}/src/events/*.c)
file(GLOB video ${SDL_DIR}/src/video/*.c)
# 需要适配 video
file(GLOB videoDummy ${SDL_DIR}/src/video/dummy/*.c)

file(GLOB atomic ${SDL_DIR}/src/atomic/*.c)
file(GLOB cpuinfo ${SDL_DIR}/src/cpuinfo/*.c)
file(GLOB file ${SDL_DIR}/src/file/*.c)
file(GLOB render ${SDL_DIR}/src/render/*.c)
file(GLOB dynapi ${SDL_DIR}/src/dynapi/*.c)
set(baseLib ${base}
            ${stdlib}
            ${timer}
            ${atomic}
            ${thread}
            ${events}
            ${video}
            ${videoDummy}
            ${cpuinfo}
            ${file}
            ${render}
            ${dynapi})
# 需要适配 sensor, 传感器
file(GLOB sensorDummy ${SDL_DIR}/src/sensor/dummy/*.c)
file(GLOB sensor ${SDL_DIR}/src/sensor/*.c)
# 需要适配 joystick # 未开放 inputEvent
file(GLOB joystickDummy ${SDL_DIR}/src/joystick/dummy/*.c ${SDL_DIR}/src/joystick/ohos/*.c ${SDL_DIR}/src/joystick/hidapi/*.c)
file(GLOB joystick ${SDL_DIR}/src/joystick/*.c )
# 需要适配 hidapi,
file(GLOB hidapi ${SDL_DIR}/src/hidapi/*.c ${SDL_DIR}/src/hidapi/ohos/*.cpp)

# 需要适配 audio, 传感器
file(GLOB audioDummy ${SDL_DIR}/src/audio/dummy/*.c ${SDL_DIR}/src/audio/ohos/*.c)
file(GLOB audio ${SDL_DIR}/src/audio/*.c)

add_library(sdl3 SHARED
    ${baseLib}
    ${hidapi}
    ${sensor}
    ${sensorDummy}
    ${joystick}
    ${joystickDummy}
    ${hidapi}
    ${audio}
    ${audioDummy}
)

target_link_libraries(sdl3 PUBLIC libace_napi.z.so libpthread.a libace_ndk.z.so libhilog_ndk.z.so libOpenSLES.so)
