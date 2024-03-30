
# 链接ffmpeg库，和opencv库， 实现rtsp流的播放
# 1. 从rtsp流中读取视频帧
# 2. 将视频帧转换为opencv的Mat格式

# cmake 版本
cmake_minimum_required(VERSION 3.0)

# 工程名
project(ffmpeg_opencv)

# 设置c++编译器
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmakes)


# find ffmpeg
find_package(FFmpeg REQUIRED)

# 添加头文件路径与库文件
include_directories(${FFMPEG_INCLUDE_DIRS})
link_directories(${FFMPEG_LIBRARIES})

# find opencv
find_package(OpenCV REQUIRED)

# 添加头文件路径与库文件
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIBRARIES})


# 添加源文件
add_executable(ffmpeg_opencv main.cpp)

# 链接库
target_link_libraries(ffmpeg_opencv avcodec avformat avutil swscale opencv_core opencv_highgui opencv_imgproc)

