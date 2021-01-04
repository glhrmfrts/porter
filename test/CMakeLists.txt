# request test
set(TARGET_NAME request_test)

file(GLOB SOURCE_FILES request_test.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/deps/curl/include/)

add_executable(${TARGET_NAME} ${SOURCE_FILES})

target_link_libraries(${TARGET_NAME} PRIVATE libcurl porter)

# client test
set(TARGET_NAME client_test)

file(GLOB SOURCE_FILES client_test.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/deps/curl/include/)

add_executable(${TARGET_NAME} ${SOURCE_FILES})

target_link_libraries(${TARGET_NAME} PRIVATE libcurl porter)