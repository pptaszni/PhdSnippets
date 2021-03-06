cmake_minimum_required(VERSION 3.11.0)
project(ImgCompression CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED COMPONENTS system program_options)
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(spdlog REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ui)

add_library(ImgCompression SHARED
  include/CameraFrameGraphicsView.hpp
  include/MainWindow.hpp
  src/AsyncFrameListener.cpp
  src/CameraFrameGraphicsView.cpp
  src/FramesFromDir.cpp
  src/HEVCVideoEncoder.cpp
  src/Logger.cpp
  src/MainWindow.cpp
  src/ROI.cpp
  src/SemanticCompression.cpp
  src/UDPClient.cpp
  src/UDPServer.cpp
  src/UnderwaterRobot.cpp
)
target_include_directories(ImgCompression PUBLIC include)
target_link_libraries(ImgCompression
  ${OpenCV_LIBRARIES}
  Qt5::Core
  Qt5::Widgets
  stdc++fs
  x265
  spdlog::spdlog
)

add_executable(compression_alg_comparison src/main.cpp)
target_link_libraries(compression_alg_comparison
  ${Boost_LIBRARIES}
  ${OpenCV_LIBRARIES}
  ImgCompression
  lz4
)

add_executable(frames_compression_gui src/gui.cpp)
target_link_libraries(frames_compression_gui
  ${Boost_LIBRARIES}
  ImgCompression
)

add_executable(frames_source src/frames_source.cpp)
target_link_libraries(frames_source
  ${Boost_LIBRARIES}
  ImgCompression
)
