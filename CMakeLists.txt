cmake_minimum_required(VERSION 3.11.0)
project(PhDSnippets CXX)

enable_testing()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(VolleyballPlayersList)
add_subdirectory(ImgCompression)
add_subdirectory(tests)
