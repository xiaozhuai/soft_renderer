cmake_minimum_required(VERSION 3.2)
project(soft_renderer)

set(CMAKE_CXX_STANDARD 14)

option(MINIFB_BUILD_EXAMPLES OFF)
add_subdirectory(vendor/minifb)

include_directories(vendor/glm)

add_subdirectory(vendor/stb)

#include(CheckCXXCompilerFlag)
#function(enable_cxx_compiler_flag_if_supported flag)
#    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
#    if (flag_already_set EQUAL -1)
#        check_cxx_compiler_flag("${flag}" flag_supported)
#        if (flag_supported)
#            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
#            message(STATUS "flag ${flag} enable")
#        else ()
#            message(STATUS "flag ${flag} not supported")
#        endif ()
#        unset(flag_supported CACHE)
#    endif ()
#endfunction()
#
#enable_cxx_compiler_flag_if_supported("-fopenmp")
#enable_cxx_compiler_flag_if_supported("-O3")

#if(APPLE)
#    if(CMAKE_C_COMPILER_ID MATCHES "Clang\$")
#        set(OpenMP_C_FLAGS "-Xpreprocessor -fopenmp")
#        set(OpenMP_C_LIB_NAMES "omp")
#        set(OpenMP_omp_LIBRARY omp)
#    endif()
#
#    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang\$")
#        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
#        set(OpenMP_CXX_LIB_NAMES "omp")
#        set(OpenMP_omp_LIBRARY omp)
#    endif()
#endif()

find_package(OpenMP REQUIRED)

file(GLOB_RECURSE SOURCES src/*.h src/*.cpp)

add_executable(soft_renderer ${SOURCES})

target_link_directories(soft_renderer PRIVATE /usr/local/opt/libomp/lib/)

target_link_libraries(soft_renderer minifb stb OpenMP::OpenMP_CXX)
