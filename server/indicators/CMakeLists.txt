cmake_minimum_required(VERSION 3.20)
project(quanux_indicators CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.11.1
)
FetchContent_MakeAvailable(pybind11)

include_directories(include)

# Python Extension
pybind11_add_module(quanux_indicators pybind/bindings.cpp)

# Install target (optional)
install(TARGETS quanux_indicators LIBRARY DESTINATION lib)
