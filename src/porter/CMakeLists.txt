set(TARGET_NAME porter)

file(GLOB SOURCE_FILES *.cpp)
file(GLOB HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*.h)

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/deps/curl/include/)

add_library(${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(${TARGET_NAME} PRIVATE libcurl)