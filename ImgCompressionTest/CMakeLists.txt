cmake_minimum_required(VERSION 3.11.0)
project(ImgCompressionTest CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV 3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system program_options)

add_library(ImgCompression SHARED
  src/FramesFromDir.cpp
  src/HEVCVideoEncoder.cpp
)
target_include_directories(ImgCompression PUBLIC include)
target_link_libraries(ImgCompression
  ${OpenCV_LIBRARIES}
  stdc++fs
  x265
)

add_executable(frames_comperssion_example src/main.cpp)
target_link_libraries(frames_comperssion_example
  ${Boost_LIBRARIES}
  ${OpenCV_LIBRARIES}
  ImgCompression
  lz4
)
