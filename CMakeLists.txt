cmake_minimum_required(VERSION 3.2)
project(soft_renderer)

set(CMAKE_CXX_STANDARD 14)

option(MINIFB_BUILD_EXAMPLES OFF)
add_subdirectory(vendor/minifb)
include_directories(vendor/glm)
option(TBB_TEST OFF)
option(TBBMALLOC_BUILD OFF)
add_subdirectory(vendor/oneTBB)
add_subdirectory(vendor/stb)

file(GLOB_RECURSE SOURCES src/*.h src/*.cpp)

add_executable(soft_renderer ${SOURCES})

target_link_libraries(soft_renderer minifb stb tbb)
