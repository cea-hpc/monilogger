cmake_minimum_required(VERSION 3.12.1)
cmake_policy(SET CMP0074 NEW)

# SET VARIABLES
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -DNDEBUG")

# PROJECT
project(libscihook CXX)
set(CMAKE_VERBOSE_MAKEFILE FALSE)

find_package(pybind11 REQUIRED)
include_directories(${pybind11_INCLUDE_DIRS} include)

set(LIBRARY_NAME scihook)
add_library(${LIBRARY_NAME} SHARED src/core/SciHook.cc)
target_link_libraries(${LIBRARY_NAME} ${PYTHON_LIBRARIES} pybind11::embed)

install(TARGETS ${LIBRARY_NAME} DESTINATION lib)
install(FILES include/SciHook.h DESTINATION include)
