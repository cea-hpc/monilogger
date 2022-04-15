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

# CHECK CXX VERSION: must be done after the project() (CMAKE_CXX_COMPILER_ID not defined before)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.1.0")
		message(FATAL_ERROR "GCC minimum required version is 9.1.0. Please upgrade.")
	endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0.0")
		message(FATAL_ERROR "Clang minimum required version is 9.0.0. Please upgrade.")
	endif()
endif()

set(PYBIND11_PYTHON_VERSION 3.8)
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
