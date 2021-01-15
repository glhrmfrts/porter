# Build libcurl
set(CURL_DISABLE_TESTS ON)
set(BUILD_SHARED_LIBS OFF)
set(BUILD_CURL_EXE OFF)
set(HTTP_ONLY ON)
set(SSL_ENABLED ON)

if (WIN32)
    set(USE_SCHANNEL ON)
    set(USE_WINDOWS_SSPI ON)
endif (WIN32)

add_subdirectory(curl)