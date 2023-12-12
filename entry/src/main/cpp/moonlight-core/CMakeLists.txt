
set(OPENSSL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openssl)
set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/openssl)
set(OPENSSL_CRYPTO_LIBRARY libcrypto)
set(OPENSSL_INCLUDE_DIR ${OPENSSL_DIR}/include)

set(CURL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/curl)
set(CURL_INCLUDE_DIR ${CURL_DIR}/include)

include_directories(
${CMAKE_CURRENT_SOURCE_DIR}
${OPENSSL_INCLUDE_DIR}
${CURL_INCLUDE_DIR}
)
# 导入静态库
add_library(libssl STATIC IMPORTED)
set_target_properties(libssl PROPERTIES IMPORTED_LOCATION ${OPENSSL_DIR}/${CMAKE_OHOS_ARCH_ABI}/libssl.a)
set_target_properties(libssl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})

# 导入静态库
add_library(libcrypto STATIC IMPORTED)
set_target_properties(libcrypto PROPERTIES IMPORTED_LOCATION ${OPENSSL_DIR}/${CMAKE_OHOS_ARCH_ABI}/libcrypto.a)
set_target_properties(libcrypto PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${OPENSSL_INCLUDE_DIR})



add_library(libcurl STATIC IMPORTED)
set_target_properties(libcurl PROPERTIES IMPORTED_LOCATION ${CURL_DIR}/${CMAKE_OHOS_ARCH_ABI}/libcurl.a)
set_target_properties(libcurl PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${CURL_INCLUDE_DIR})

add_subdirectory(moonlight-common-c)



add_library(entry SHARED
hello.cpp
moon_bridge.cpp
x509Utils.cpp
http_curl.cpp
)


target_link_libraries(entry PRIVATE libssl ${OPENSSL_CRYPTO_LIBRARY} moonlight-common-c )

target_include_directories(entry SYSTEM PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}
${OPENSSL_INCLUDE_DIR}
${CURL_INCLUDE_DIR}
${CMAKE_CURRENT_SOURCE_DIR}/moonlight-common-c/reedsolomon
${CMAKE_CURRENT_SOURCE_DIR}/moonlight-common-c/enet/include
)

target_link_libraries(entry PRIVATE
libace_napi.z.so
libhilog_ndk.z.so
libcurl
libz.so)

