cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

# SET VARIABLES
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

# PROJECT
project(scihook CXX)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(Python_FIND_VIRTUALENV ONLY)

find_package(Python COMPONENTS Interpreter Development REQUIRED)
set(pybind11_DIR "${Python_SITELIB}/pybind11/share/cmake/pybind11")
find_package(pybind11 REQUIRED)
include_directories(${pybind11_INCLUDE_DIRS} scihook/include)

add_library(scihook SHARED SciHook.cc)
target_link_libraries(scihook ${PYTHON_LIBRARIES} pybind11::embed)

# Python extension
pybind11_add_module(_scihook scihook_module.cc)
set_target_properties(_scihook PROPERTIES
        BUILD_WITH_INSTALL_RPATH FALSE
        LINK_FLAGS "-Wl,-rpath,$ORIGIN/")
target_link_libraries(_scihook PUBLIC scihook)
