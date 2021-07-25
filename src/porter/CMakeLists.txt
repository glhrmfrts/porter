set(TARGET_NAME porter)

file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*.h)

add_library(${TARGET_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})

include_directories(${PROJECT_SOURCE_DIR}/include/)

if (WIN32)
    include_directories(${PROJECT_SOURCE_DIR}/libs/openssl-win32/include/)
    target_link_directories(${TARGET_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/libs/openssl-win32/lib/)
    target_link_libraries(${TARGET_NAME} PRIVATE libcurl crypt32)
endif (WIN32)

if (UNIX)
    target_link_libraries(${TARGET_NAME} PRIVATE curl expat util)
endif (UNIX)