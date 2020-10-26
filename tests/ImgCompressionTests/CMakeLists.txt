add_executable(ut-imgcompression
  main.cpp
  NetworkTests.cpp
)
target_link_libraries(ut-imgcompression
  gmock
  ImgCompression
)
add_test(ImgCompressionTests ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/ut-imgcompression)
