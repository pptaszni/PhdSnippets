cmake_minimum_required(VERSION 3.11.0)
project(VolleyballPlayersList CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(nlohmann_json REQUIRED)

add_executable(vb_players src/main.cpp)
target_link_libraries(vb_players PUBLIC nlohmann_json::nlohmann_json)
