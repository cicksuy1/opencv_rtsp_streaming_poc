cmake_minimum_required(VERSION 3.7)
project(rtsp-stream)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

add_library(RTSPStreamerLib
    ${PROJECT_SOURCE_DIR}/include/rtsp-stream.cpp
)

add_executable(rtsp-stream
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

target_link_libraries(rtsp-stream
    RTSPStreamerLib
    ${OpenCV_LIBS}
    pthread
)
