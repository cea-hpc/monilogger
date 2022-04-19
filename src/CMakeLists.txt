cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0074 NEW)

# SET VARIABLES
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3")

# PROJECT
project(monilog CXX)
set(CMAKE_VERBOSE_MAKEFILE TRUE)

set(PYBIND11_PYTHON_VERSION 3.8)
find_package(Python COMPONENTS Interpreter Development REQUIRED)
set(pybind11_DIR "${Python_SITELIB}/pybind11/share/cmake/pybind11")
find_package(pybind11 REQUIRED)
include_directories(${pybind11_INCLUDE_DIRS} monilog/include)

add_library(monilog SHARED MoniLog.cc)
target_link_libraries(monilog ${PYTHON_LIBRARIES} pybind11::embed)


# Python extension
pybind11_add_module(_monilog monilog_module.cc)
set_target_properties(_monilog PROPERTIES
        BUILD_WITH_INSTALL_RPATH FALSE
        LINK_FLAGS "-Wl,-rpath,$ORIGIN/")
target_link_libraries(_monilog PUBLIC monilog)
