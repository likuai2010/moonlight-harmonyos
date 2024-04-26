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


set(LIBS_DIR ${NATIVERENDER_ROOT_PATH}/libs)

set(OPENSSL_ROOT_DIR ${LIBS_DIR}/openssl)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
set(CURL_DIR ${LIBS_DIR}/curl)
set(CURL_INCLUDE_DIR ${CURL_DIR}/include)


# 导入静态库
add_library(libssl STATIC IMPORTED)
set_target_properties(libssl PROPERTIES IMPORTED_LOCATION ${OPENSSL_ROOT_DIR}/${CMAKE_OHOS_ARCH_ABI}/libssl.a)
set_target_properties(libssl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})

# 导入静态库
add_library(libcrypto STATIC IMPORTED)
set_target_properties(libcrypto PROPERTIES IMPORTED_LOCATION ${OPENSSL_ROOT_DIR}/${CMAKE_OHOS_ARCH_ABI}/libcrypto.a)
set_target_properties(libcrypto PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})


add_subdirectory(utils)

add_subdirectory(moonlight-core)