cmake_minimum_required(VERSION 3.12.1)
cmake_policy(SET CMP0074 NEW)

# SET VARIABLES
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -DNDEBUG")

# PROJECT
project(scihook CXX)
set(CMAKE_VERBOSE_MAKEFILE TRUE)

find_package(pybind11 REQUIRED)
include_directories(${pybind11_INCLUDE_DIRS})


if(SCIHOOK_ROOT)
    message(STATUS "Looking for libscihook.so in ${SCIHOOK_ROOT}/lib")
    find_library(SCIHOOK_LIB scihook PATHS ${SCIHOOK_ROOT}/lib)
else()
    find_library(SCIHOOK_LIB scihook)
endif()
if(SCIHOOK_LIB)
    message(STATUS "libscihook found at: ${SCIHOOK_LIB}")
    cmake_path(GET SCIHOOK_LIB PARENT_PATH SCIHOOK_LIB_DIR)
    cmake_path(GET SCIHOOK_LIB_DIR PARENT_PATH SCIHOOK_ROOT_DIR)
    set(SCIHOOK_INCLUDE_DIR ${SCIHOOK_ROOT_DIR}/include)
    find_file(SCIHOOK_INCLUDE SciHook.h PATHS ${SCIHOOK_INCLUDE_DIR})
    if (SCIHOOK_INCLUDE)
        message(STATUS "SciHook.h found at: ${SCIHOOK_INCLUDE}")
    else()
        message(FATAL_ERROR "Could not find SciHook.h in ${SCIHOOK_INCLUDE_DIR}")
    endif()
    include_directories(${SCIHOOK_INCLUDE_DIR})
else()
    message(FATAL_ERROR "Could not find libscihook.so")
endif()

# Python extension
pybind11_add_module(_scihook scihook_module.cc)
target_link_libraries(_scihook PUBLIC ${SCIHOOK_LIB})
install(TARGETS _scihook DESTINATION .)
