cmake_minimum_required(VERSION 3.2)
project(stb)

set(CMAKE_CXX_STANDARD 11)

add_library(stb STATIC include/stb_image.h include/stb_image_write.h src/impl.cpp)
set_target_properties(stb PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/include")
