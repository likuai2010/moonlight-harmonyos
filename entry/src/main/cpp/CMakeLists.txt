# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(moonlight)

add_definitions(-DOHOS_PLATFORM)

#sdk
include_directories(
    ${NATIVERENDER_ROOT_PATH}
    ${NATIVERENDER_ROOT_PATH}/include
)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_subdirectory(moonlight-core)