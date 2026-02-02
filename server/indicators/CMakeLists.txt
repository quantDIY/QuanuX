cmake_minimum_required(VERSION 3.15)
project(quanux_indicators VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Header-only library for the core
add_library(quanux_indicators_lib INTERFACE)

target_include_directories(quanux_indicators_lib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/contrib>
    $<INSTALL_INTERFACE:include>
)

# Enable compiler warnings
if(MSVC)
    target_compile_options(quanux_indicators_lib INTERFACE /W4 /WX)
else()
    target_compile_options(quanux_indicators_lib INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif()

# Tests
if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()


# Python bindings are now handled via Cython in ./cython/setup.py
# Run: cd cython && python3 setup.py build_ext --inplace
