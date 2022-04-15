cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

# SET VARIABLES
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 17)

# PROJECT
project(monilog CXX)

set(CMAKE_VERBOSE_MAKEFILE FALSE)
set(PYBIND11_PYTHON_VERSION 3.8)

find_package(pybind11 REQUIRED)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
include_directories(${pybind11_INCLUDE_DIRS} ${Python_SITELIB}/monilog/include)

# EXECUTABLE simpleembedding
add_executable(simpleembedding SimpleEmbedding.cc)
target_include_directories(simpleembedding PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
add_library(moniloglib SHARED IMPORTED)
set_property(TARGET moniloglib PROPERTY IMPORTED_LOCATION ${Python_SITELIB}/monilog/libmonilog.so)
target_link_libraries(simpleembedding PUBLIC pybind11::embed moniloglib)
