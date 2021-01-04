# Build libcurl
set(BUILD_SHARED_LIBS OFF)
set(BUILD_CURL_EXE OFF)
set(HTTP_ONLY ON)
set(CURL_DISABLE_TESTS ON)
add_subdirectory(curl)