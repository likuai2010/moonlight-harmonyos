

set(LIBS_DIR ${NATIVERENDER_ROOT_PATH}/libs)

set(OPENSSL_ROOT_DIR ${LIBS_DIR}/openssl)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
set(CURL_DIR ${LIBS_DIR}/curl)
set(CURL_INCLUDE_DIR ${CURL_DIR}/include)


include_directories(
    ${CURL_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)



add_library(libcurl SHARED IMPORTED)
set_target_properties(libcurl PROPERTIES IMPORTED_LOCATION ${CURL_DIR}/${CMAKE_OHOS_ARCH_ABI}/libcurl-d.so)
set_target_properties(libcurl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CURL_INCLUDE_DIR})


add_library(cUtils SHARED
    utils.cpp
    x509Utils.cpp
    http_curl.cpp
)
target_include_directories(cUtils SYSTEM PRIVATE
    ${CURL_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

target_link_libraries(cUtils PRIVATE
    libssl
    libcrypto
    libcurl
    ace_ndk.z.so
    libace_napi.z.so
    libhilog_ndk.z.so
    libuv.so
    libz.so
    libc++.a
)



