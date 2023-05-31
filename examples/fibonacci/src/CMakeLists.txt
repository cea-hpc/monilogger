cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

# SET VARIABLES
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 17)

# PROJECT
project(fibonacci CXX)

set(CMAKE_VERBOSE_MAKEFILE FALSE)
set(PYBIND11_PYTHON_VERSION 3.8)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
set(pybind11_DIR "${Python_SITELIB}/pybind11/share/cmake/pybind11")
find_package(pybind11 REQUIRED)
include_directories(${pybind11_INCLUDE_DIRS} ${Python_SITELIB}/scihook/include)

# EXECUTABLE fibonacci
add_executable(fibonacci fibonacci_main.cc fibonacci_embedding.cc fibonacci.cc)
target_include_directories(fibonacci PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
add_library(scihooklib SHARED IMPORTED)
set_property(TARGET scihooklib PROPERTY IMPORTED_LOCATION ${Python_SITELIB}/scihook/libscihook.so)
target_link_libraries(fibonacci PUBLIC pybind11::embed scihooklib)
