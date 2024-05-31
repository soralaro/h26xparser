cmake_minimum_required(VERSION 3.5)

project(h26xparser)
set(CMAKE_CXX_STANDARD 11)
include_directories(${PROJECT_SOURCE_DIR}/)
include_directories(${PROJECT_SOURCE_DIR}/media/h265)
include_directories(${PROJECT_SOURCE_DIR}/media/h264)
#link_directories(${PROJECT_SOURCE_DIR}/media/h264)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS_DEBUG   "$ENV{CXXFLAGS} -O0 -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -fpermissive -fno-strict-aliasing -fopenmp")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall  -fopenmp")

set (TARGET_NAME "h26xparser")

#file(GLOB_RECURSE cpp_files "./media/h264/*.cpp")
#file(GLOB_RECURSE cpp_files "./media/h265/*.cpp")

list(APPEND SRC_FILES
        ${PROJECT_SOURCE_DIR}/media/h264/bitstream.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-hrd.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-nal.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-parser.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-pps.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-profile.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-rbsp.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-scaling.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-sei.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-slice-header.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-sps.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-stream.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-vui.c
        ${PROJECT_SOURCE_DIR}/media/h264/h264-util.h
        ${PROJECT_SOURCE_DIR}/media/h265/h265-hrd.c
        ${PROJECT_SOURCE_DIR}/media/h265/h265-nal.c
        ${PROJECT_SOURCE_DIR}/media/h265/h265-parser.c
        ${PROJECT_SOURCE_DIR}/media/h265/h265-pps.c
        ${PROJECT_SOURCE_DIR}/media/h265/h265-sps.c
        ${PROJECT_SOURCE_DIR}/media/h265/h265-vps.c
        ${PROJECT_SOURCE_DIR}/media/h265/h265-vui.c
        ${PROJECT_SOURCE_DIR}/interface.cpp
        )
#list(APPEND SRC_FILES vDecProc.cpp main.cpp)
#add_library(${TARGET_NAME} SHARED h264_parser.c)
add_library(${TARGET_NAME} SHARED ${SRC_FILES})
#add_executable(${TARGET_NAME} ${SRC_FILES})
#add_executable(${TARGET_NAME} ${cpp_files})
add_executable(main "main.cpp")
target_link_libraries(main PRIVATE  -lh26xparser)
#add_executable(test "main.cpp" "ffmpeg_plugin.cpp" "vDecProc.cpp")

#target_link_libraries(${TARGET_NAME} PRIVATE -lavcodec -lavformat -lavutil -lswscale  -lopencv_core -lopencv_highgui  -lopencv_imgproc -lopencv_imgcodecs)
##target_link_libraries(test PRIVATE -lavcodec -lavformat -lavutil -lswscale  -lopencv_core -lopencv_highgui  -lopencv_imgproc -lopencv_imgcodecs -ldl -lffmpeg_plugin)
#target_link_libraries(test PRIVATE -lavcodec -lavformat -lavutil -lswscale  -lopencv_core -lopencv_highgui  -lopencv_imgproc -lopencv_imgcodecs -ldl)
