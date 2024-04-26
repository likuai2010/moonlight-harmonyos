

set(LIBS_DIR ${NATIVERENDER_ROOT_PATH}/libs)
# moonlight-common-c 需要指定SSL目录
set(OPENSSL_ROOT_DIR ${LIBS_DIR}/openssl)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
set(OPENSSL_CRYPTO_LIBRARY libcrypto)

set(CURL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/curl)
set(CURL_INCLUDE_DIR ${CURL_DIR}/include)
set(FFmpeg OFF)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/opus/include
    ${OPENSSL_INCLUDE_DIR}
    ${LIBS_DIR}/ffmpeg/include
    ${LIBS_DIR}/sdl/sdl-ohos/include
)

if (FFmpeg)
    include_directories(${LIBS_DIR}/ffmpeg/include)
    add_library(ffmpeg SHARED IMPORTED)
    set_target_properties(ffmpeg PROPERTIES IMPORTED_LOCATION ${LIBS_DIR}/ffmpeg/${CMAKE_OHOS_ARCH_ABI}/libohosffmpeg.z.so)
endif()


add_subdirectory(opus)
add_subdirectory(moonlight-common-c)

add_library(entry SHARED
    hello.cpp
    audio/Audio.cpp
    audio/DataQueue.cpp
    audio/PcmData.cpp
    Shader.cpp
    moon_bridge.cpp
    video/VideoRender.cpp
    video/AVFrameHolder.cpp
    video/decoder/NativeVideoDecoder.cpp
    video/render/eglRender.cpp
    video/render/plugin_render.cpp
    audio/SDLAudioRenderer.cpp
)

if (FFmpeg)
    target_compile_definitions(entry PRIVATE FFMPEG_ENABLED)
    target_sources(entry PRIVATE video/decoder/FFmpegVideoDecoder.cpp)
    target_link_libraries(entry PRIVATE ffmpeg)
endif()



target_include_directories(entry SYSTEM PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/opus/include
    ${CMAKE_CURRENT_SOURCE_DIR}/moonlight-common-c/reedsolomon
    ${CMAKE_CURRENT_SOURCE_DIR}/moonlight-common-c/enet/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/include
)

target_link_libraries(entry PRIVATE
    opus
    moonlight-common-c
    ace_ndk.z.so
    libace_napi.z.so
    libhilog_ndk.z.so
    libnative_media_codecbase.so
    libnative_media_core.so
    libnative_media_vdec.so
    libEGL.so
    GLESv3.so
    libOpenSLES.so
    libuv.so
    libz.so
    libc++.a
)

target_link_libraries(entry PRIVATE cUtils)

