
set(OPENSSL_LIBRARIES libssl libcrypto)
project(CURL C)
function(transform_makefile_inc INPUT_FILE OUTPUT_FILE)
  file(READ ${INPUT_FILE} MAKEFILE_INC_TEXT)
  string(REPLACE "$(top_srcdir)"   "\${CURL_SOURCE_DIR}" MAKEFILE_INC_TEXT ${MAKEFILE_INC_TEXT})
  string(REPLACE "$(top_builddir)" "\${CURL_BINARY_DIR}" MAKEFILE_INC_TEXT ${MAKEFILE_INC_TEXT})

  string(REGEX REPLACE "\\\\\n" "!π!α!" MAKEFILE_INC_TEXT ${MAKEFILE_INC_TEXT})
  string(REGEX REPLACE "([a-zA-Z_][a-zA-Z0-9_]*)[\t ]*=[\t ]*([^\n]*)" "SET(\\1 \\2)" MAKEFILE_INC_TEXT ${MAKEFILE_INC_TEXT})
  string(REPLACE "!π!α!" "\n" MAKEFILE_INC_TEXT ${MAKEFILE_INC_TEXT})

  string(REGEX REPLACE "\\$\\(([a-zA-Z_][a-zA-Z0-9_]*)\\)" "\${\\1}" MAKEFILE_INC_TEXT ${MAKEFILE_INC_TEXT})    # Replace $() with ${}
  string(REGEX REPLACE "@([a-zA-Z_][a-zA-Z0-9_]*)@" "\${\\1}" MAKEFILE_INC_TEXT ${MAKEFILE_INC_TEXT})    # Replace @@ with ${}, even if that may not be read by CMake scripts.
  file(WRITE ${OUTPUT_FILE} ${MAKEFILE_INC_TEXT})
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${INPUT_FILE}")
endfunction()
set(LIB_SHARED "libcurl_shared")
set(BUILD_SHARED_LIBS ON)
set(LIB_SELECTED ${LIB_SHARED})
add_subdirectory(curl-8.5.0/lib)