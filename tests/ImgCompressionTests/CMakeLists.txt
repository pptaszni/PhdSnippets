find_package(Qt5 COMPONENTS Test REQUIRED)

add_executable(ut-imgcompression
  AsyncWorkerTests.cpp
  main.cpp
  MainWindowTests.cpp
  NetworkTests.cpp
  Reproduction.cpp
)
target_link_libraries(ut-imgcompression
  gmock
  ImgCompression
  Qt5::Test
)
add_test(ImgCompressionTests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ut-imgcompression)
