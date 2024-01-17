
set(OPENSSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openssl)
set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openssl)
set(OPENSSL_CRYPTO_LIBRARY libcrypto)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_DIR}/include)

set(CURL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/curl)
set(CURL_INCLUDE_DIR ${CURL_DIR}/include)
set(FFmpeg True)

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/sdl/sdl-ohos/include
${CMAKE_CURRENT_SOURCE_DIR}/opus/include
${OPENSSL_INCLUDE_DIR}
${CURL_INCLUDE_DIR}
)

if (FFmpeg)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/include)
    add_library(ffmpeg SHARED IMPORTED)
    set_target_properties(ffmpeg PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/${CMAKE_OHOS_ARCH_ABI}/libohosffmpeg.z.so)
endif()

# 导入静态库
add_library(libssl STATIC IMPORTED)
set_target_properties(libssl PROPERTIES IMPORTED_LOCATION ${OPENSSL_DIR}/${CMAKE_OHOS_ARCH_ABI}/libssl.a)
set_target_properties(libssl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})

# 导入静态库
add_library(libcrypto STATIC IMPORTED)
set_target_properties(libcrypto PROPERTIES IMPORTED_LOCATION ${OPENSSL_DIR}/${CMAKE_OHOS_ARCH_ABI}/libcrypto.a)
set_target_properties(libcrypto PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})


add_library(libcurl SHARED IMPORTED)
set_target_properties(libcurl PROPERTIES IMPORTED_LOCATION ${CURL_DIR}/${CMAKE_OHOS_ARCH_ABI}/libcurl-d.so)
set_target_properties(libcurl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CURL_INCLUDE_DIR})



#set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

add_subdirectory(opus)
add_subdirectory(sdl)
add_subdirectory(moonlight-common-c)

#set(OPENSSL_LIBRARIES libssl libcrypto)
#add_subdirectory(curl/curl-8.5.0)

add_library(entry SHARED
hello.cpp
audio/Audio.cpp
audio/DataQueue.cpp
audio/PcmData.cpp
Shader.cpp
moon_bridge.cpp
utils/x509Utils.cpp
utils/http_curl.cpp
video/AVFrameHolder.cpp
#video/NativeVideoDecoder.cpp
video/render/eglRender.cpp
video/render/plugin_render.cpp
audio/SDLAudioRenderer.cpp
)

if (FFmpeg)
    target_compile_definitions(entry PRIVATE FFMPEG_ENABLED)
    target_sources(entry PRIVATE video/FFmpegVideoDecoder.cpp)
    target_link_libraries(entry PRIVATE ffmpeg)
endif()

target_link_libraries(entry PRIVATE
libssl
libcrypto
opus
sdl3
moonlight-common-c
libcurl
)

target_include_directories(entry SYSTEM PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}
${OPENSSL_INCLUDE_DIR}
${CURL_INCLUDE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/opus/include
${CMAKE_CURRENT_SOURCE_DIR}/moonlight-common-c/reedsolomon
${CMAKE_CURRENT_SOURCE_DIR}/moonlight-common-c/enet/include
)

target_include_directories(entry PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ffmpeg/include)

target_link_libraries(entry PRIVATE
ace_ndk.z.so
libace_napi.z.so
libhilog_ndk.z.so
#libnative_media_codecbase.so
# libnative_media_core.so
#libnative_media_vdec.so
EGL.so
GLESv3.so
libuv.so
libz.so
libc++.a
)

